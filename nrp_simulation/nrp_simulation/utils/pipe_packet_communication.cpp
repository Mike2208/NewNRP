#include "nrp_simulation/utils/pipe_packet_communication.h"

#include <assert.h>
#include <iostream>
#include <limits>
#include <string.h>
#include <sys/signalfd.h>
#include <unistd.h>

PipeCommPacket PipeCommPacket::readPipePacket(PipePacketCommunication &comm, uint16_t numTries, uint16_t waitTime)
{
	PipeCommPacket packet;

	// Read header and data size
	if(const auto readBytes = comm.readP(&packet, PipeCommPacket::HeaderSize, numTries, waitTime) < PipeCommPacket::HeaderSize)
	{
		if(readBytes > 0)
		{
			std::cerr << "Read only part of a packet's static header";
		}

		packet.ID = 0;
		return packet;
	}

	// Read header command
	packet.Command.resize(packet.CommandLength);

	if(const auto readBytes = comm.readP(packet.Command.data(), packet.CommandLength, numTries, waitTime) < packet.CommandLength)
	{
		if(readBytes > 0)
		{
			std::cerr << "Read only part of a packet's command";
		}

		packet.ID = 0;
		return packet;
	}

	// Read data
	packet.Data.resize(packet.DataLength);

	if(const auto readBytes = comm.readP(packet.Data.data(), packet.Data.size(), numTries, waitTime) < packet.Data.size())
	{
		if(readBytes > 0)
		{
			std::cerr << "Read only part of a packet's data";
		}

		packet.ID = 0;
		return packet;
	}

	return packet;
}

bool PipeCommPacket::writePipePacket(PipePacketCommunication &comm, PipeCommPacket &packet, uint16_t numTries, uint16_t waitTime)
{
	packet.CommandLength = packet.Command.size()+1;
	packet.DataLength = packet.Data.size();

	if(comm.writeP(&packet, PipeCommPacket::HeaderSize, numTries, waitTime) < PipeCommPacket::HeaderSize)
	{
		std::cerr << "Failed to write packet header with ID " << packet.ID << std::endl;
		return false;
	}

	if(comm.writeP(packet.Command.data(), packet.CommandLength, numTries, waitTime) < packet.CommandLength)
	{
		std::cerr << "Failed to write packet command \"" << packet.Command << "\"\n";
		return false;
	}

	if(comm.writeP(packet.Data.data(), packet.DataLength, numTries, waitTime) < packet.DataLength)
	{
		std::cerr << "Failed to write packet data" << std::endl;
		return false;
	}

	return  true;
}

PipePacketCommunication::PipePacketCommunication(PipeCommunication &&comm, pid_t commPID, uint16_t confirmSignalOffsetNum)
    : PipeCommunication(std::move(comm)),
      _confirmSignalOffsetNum(confirmSignalOffsetNum),
      _commPID(commPID)
{
	this->startServerAsync();
}

PipePacketCommunication::~PipePacketCommunication()
{
	this->shutdownServer();
}

void PipePacketCommunication::blockResetSignal(uint16_t confirmSignalOffsetNum)
{
	// Block signal from main thread
	const auto resetSignal = PipePacketCommunication::getResetSignalNum(confirmSignalOffsetNum);
	const auto signal = PipePacketCommunication::generateSigset(resetSignal);

	if(sigprocmask(SIG_BLOCK, &signal, nullptr) != 0)
	{
		const auto errMsg = "Could not block proc communication signal " + std::to_string(resetSignal) + ":\n" + strerror(errno);
		std::cerr << errMsg << std::endl;

		throw std::runtime_error(errMsg);
	}
}

void PipePacketCommunication::startServerAsync()
{
	if(!this->_commRunning)
	{
		assert(!this->_commThread.joinable());

		this->_commRunning = true;
		this->_commThread = std::thread(&PipePacketCommunication::commHandler, this);
	}
}

void PipePacketCommunication::shutdownServer()
{
	if(this->_commRunning)
	{
		assert(this->_commThread.joinable());

		// Block signal from main thread. Will also affect any threads created by this one
		PipePacketCommunication::blockResetSignal(this->_confirmSignalOffsetNum);

		// Stop communication thread and wait for it to complete
		this->_commRunning = false;
		this->_commThread.join();
	}
}

bool PipePacketCommunication::isRunning() const
{
	return this->_commRunning;
}

uint16_t PipePacketCommunication::confirmSignalOffsetNum() const
{	return this->_confirmSignalOffsetNum;	}

int PipePacketCommunication::resetSignalNum() const
{	return getResetSignalNum(this->_confirmSignalOffsetNum);	}

PipeCommPacket::packet_id_t PipePacketCommunication::issuePackID()
{
	if(this->_nextPackID <= 0)
		this->_nextPackID = 1;

	return this->_nextPackID++;
}

PipeCommPacket::packet_id_t PipePacketCommunication::sendPacket(PipeCommPacket &&packet)
{
	packet.CommandLength = packet.Command.size()+1;
	packet.DataLength = packet.Data.size();

	if(packet.ID <= 0)
		packet.ID = this->issuePackID();

	PipeCommPacket::packet_id_t retVal = packet.ID;

	lock_t lock(this->_inPackLock);
	this->_inPackets.push_back(std::move(packet));

	return retVal;
}

PipeCommPacket::packet_id_t PipePacketCommunication::sendPacketImmediately(PipeCommPacket &&packet)
{
	bool restartServer;
	{
		lock_t lock(this->_outPackLock);

		restartServer  = this->isRunning();

		// Stop server if it's running
		if(restartServer)
			this->shutdownServer();

		// Assign ID if requested
		if(packet.ID < 0)
			packet.ID = this->issuePackID();

		// Send packet
		if(!PipeCommPacket::writePipePacket(*this, packet, CommWriteTries, CommWriteWaitTime))
			packet.ID = -1;
	}

	// Restart server if previously running
	if(restartServer)
		this->startServerAsync();

	return packet.ID;
}

PipePacketCommunication::lock_t PipePacketCommunication::acquireInPacketLock()
{
	return lock_t(this->_inPackLock);
}

bool PipePacketCommunication::readPacketReady() const
{
	return !this->_inPackets.empty();
}

PipeCommPacket &&PipePacketCommunication::retrievePacket()
{
	return std::get<0>(this->retrievePacket(this->acquireInPacketLock()));
}

std::tuple<PipeCommPacket &&, PipePacketCommunication::lock_t> PipePacketCommunication::retrievePacket(PipePacketCommunication::lock_t &&inPacketLock)
{
	if(this->_inPackets.empty())
		throw PipePacketCommunication::no_packets();

	PipeCommPacket pack = std::move(this->_inPackets.front());
	this->_inPackets.erase(this->_inPackets.begin());

	return {std::move(pack), std::move(inPacketLock)};
}

void PipePacketCommunication::commHandler()
{
	// Setup signal handler
	const auto sigNum = PipePacketCommunication::getResetSignalNum(this->_confirmSignalOffsetNum);
	const auto signal = PipePacketCommunication::generateSigset(sigNum);
	const auto sigFd = signalfd(-1, &signal, SFD_NONBLOCK);
	if(sigFd < 0)
	{
		const auto errMsg = std::string("Could not create signalfd:\n") + strerror(errno);
		std::cerr << errMsg << std::endl;

		throw std::runtime_error(errMsg);
	}

	auto outIter = this->_outPackets.begin();

	// Continue handling data until comm is shut down
	while(this->_commRunning)
	{
		// Read available packets
		{
			PipeCommPacket recPacket = PipeCommPacket::readPipePacket(*this, CommReadTries, CommReadWaitTime);
			if(recPacket.ID > 0)
			{
				{
					lock_t lock(this->_inPackLock);
					this->_inPackets.push_back(std::move(recPacket));
				}

				sigqueue(this->_commPID, sigNum, {recPacket.ID});
			}
		}

		// Send any packets
		if(!this->_outPackets.empty())
		{
			lock_t lock(this->_outPackLock);

			uint16_t sentPackets = 0;
			while(outIter != this->_outPackets.end() && sentPackets < CommWritePackets)
			{
				if(!PipeCommPacket::writePipePacket(*this, *outIter, CommWriteTries, CommWriteWaitTime))
					std::cerr << "Unable to send packet with ID " << outIter->ID << std::endl;
				else
					outIter++;

				sentPackets++;
			}
		}

		// Check that packets were received
		constexpr size_t SigSize = sizeof(signalfd_siginfo);

		// Check signalfd and remove confirmed packets from _outPackets
		size_t readData;
		signalfd_siginfo signalDat;
		while(ssize_t readBytes = read(sigFd, &signalDat, SigSize > 0))
		{
			// Make sure entire signalfd_siginfo struct was read
			readData = static_cast<size_t>(readBytes);
			while(readData < SigSize)
			{
				readBytes = read(sigFd, reinterpret_cast<uint8_t*>(&signalDat)+readData, SigSize - readData);
				if(readBytes < 0)
				{
					const auto errMsg = std::string("Unable to read from signalfd:\n") + strerror(errno);
					std::cerr << errMsg << std::endl;

					throw std::runtime_error(errMsg);
				}

				readData += static_cast<size_t>(readBytes);
			}

			// Handle signal, ignore signals not sent by communication partner
			if(static_cast<int>(signalDat.ssi_pid) != this->_commPID)
				continue;

			assert(static_cast<int>(signalDat.ssi_signo) == sigNum);

			{
				// Find packet to erase
				lock_t lock(this->_outPackLock);
				bool packFound = false;
				for(auto curIt = this->_outPackets.begin(); curIt != this->_outPackets.end() && curIt != outIter; ++curIt)
				{
					if(signalDat.ssi_int == curIt->ID)
					{
						packFound = true;
						this->_outPackets.erase(curIt);

						break;
					}
				}

				if(!packFound)
					std::cerr << "Received signal receipt for unsent ID " << signalDat.ssi_int << std::endl;
			}
		}
	}
}
