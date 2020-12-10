/* * NRP Core - Backend infrastructure to synchronize simulations
 *
 * Copyright 2020 Michael Zechmair
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This project has received funding from the European Union’s Horizon 2020
 * Framework Programme for Research and Innovation under the Specific Grant
 * Agreement No. 945539 (Human Brain Project SGA3).
 */

#ifndef PIPE_PACKET_COMMUNICATION_H
#define PIPE_PACKET_COMMUNICATION_H

#include "nrp_general_library/utils/pipe_communication.h"

#include <functional>
#include <list>
#include <map>
#include <mutex>
#include <signal.h>
#include <thread>
#include <vector>

class PipePacketCommunication;

/*!
 * \brief Pipe Packet.
 * Header:
 * - size_t CommandLength
 * - size_t DataLength
 * - uint32_t ID
 *
 * Data:
 * - std::string Command
 * - std::vector<uint8_t> Data
 */
struct PipeCommPacket
{
	using data_t = std::vector<uint8_t>;
	using packet_id_t = int32_t;

	/*!
	 * \brief Size of command string (including terminating '\0')
	 */
	size_t CommandLength;

	/*!
	 * \brief Size of Packet Data
	 */
	size_t DataLength;

	/*!
	 * \brief Packet ID
	 */
	packet_id_t ID = 0;

	static constexpr auto HeaderSize = sizeof(CommandLength) + sizeof(DataLength) + sizeof(ID);

	/*!
	 * \brief Command to execute
	 */
	std::string Command;

	/*!
	 * \brief Packet Data
	 */
	data_t Data;

	/*!
	 * \brief Read a packet from comm
	 * \param comm Pipe to use for communication
	 * \param numTries How often to retry a read
	 * \param waitTime Time (in seconds) to wait between read tries
	 * \return Returns read sim packet. If nothing was received, ID will be 0
	 */
	static PipeCommPacket readPipePacket(PipePacketCommunication &comm, uint16_t numTries = 1, uint16_t waitTime = 0);

	/*!
	 * \brief Write a packet to comm. Will adjust packet::CommandLength and packet::DataLength before sending
	 * \param comm Pipe to use for communication
	 * \param packet Packet to send
	 * \param numTries How often to retry a read
	 * \param waitTime Time (in seconds) to wait between read tries
	 * \return Returns true on success, fail otherwise
	 */
	static bool writePipePacket(PipePacketCommunication &comm, PipeCommPacket &packet, uint16_t numTries = 1, uint16_t waitTime = 0);
};

/*!
 * \brief Communicate over pipes with data packets
 */
class PipePacketCommunication
        : protected PipeCommunication
{
		static constexpr uint16_t CommReadTries = 2;
		static constexpr uint16_t CommReadWaitTime = 1;

		static constexpr uint16_t CommWriteTries = 2;
		static constexpr uint16_t CommWriteWaitTime = 1;

		/*!
		 * \brief How many packets should be written at once
		 */
		static constexpr uint16_t CommWritePackets = 5;

	public:
		/*!
		 * \brief Exception that will be thrown if no packet is available at retrievePacket
		 */
		struct no_packets
		{};

		/*!
		 * \brief Callback function. Registered to a specific ID
		 */
		using callback_fcn_f = std::function<PipeCommPacket(PipeCommPacket&&)>;

		using mutex_t = std::mutex;
		using lock_t = std::unique_lock<mutex_t>;


		/*!
		 * \brief Constructor. Starts server and blocks subsequent threads from handling resetSignal
		 * \param comm Communication Pipes
		 * \param commPID PID of communication partner
		 * \param confirmSignalOffsetNum reset Signal (will be offset from SIGRTMIN)
		 */
		PipePacketCommunication(PipeCommunication &&comm, pid_t commPID, uint16_t confirmSignalOffsetNum);
		~PipePacketCommunication();

		/*!
		 * \brief Block current thread from handling given signal. Will also affect any threads created from this one.
		 * Should be called at process start, before any other threads are spawned
		 * \param confirmSignalOffsetNum Reset Signal offset from SIGRTMIN
		 */
		static void blockResetSignal(uint16_t confirmSignalOffsetNum);

		void startServerAsync();
		void shutdownServer();

		bool isRunning() const;

		/*!
		 * \brief Get confirmSignalOffsetNum
		 */
		uint16_t confirmSignalOffsetNum() const;

		/*!
		 * \brief Get resetSignalNum
		 */
		int resetSignalNum() const;

		/*!
		 * \brief Return a new valid Packet ID. Will iterate from 1 to INT_MAX, then restart at 1
		 */
		PipeCommPacket::packet_id_t issuePackID();

		/*!
		 * \brief Add a packet to send queue (_outPackets). Adjusts Command- and DataLength.
		 * If packet.ID <= 0, a new ID will be issued
		 * \param packet Packet to send
		 * \return Returns ID of packet
		 */
		PipeCommPacket::packet_id_t sendPacket(PipeCommPacket &&packet);

		/*!
		 * \brief Send a packet immediately. Blocks until packet is sent. Adjusts Command- and DataLength.
		 * If packet.ID <= 0, a new ID will be issued
		 * \param packet Packet to send
		 * \return Returns ID of packet. If ID < 0, packet could not be sent
		 */
		PipeCommPacket::packet_id_t sendPacketImmediately(PipeCommPacket &&packet);

		/*!
		 * \brief Lock retrieved packets queue
		 */
		lock_t acquireInPacketLock();

		/*!
		 * \brief Returns true if a packet is in _inPackets, false otherwise
		 */
		bool readPacketReady() const;

		/*!
		 * \brief Pops available packet from _inPackets
		 * \exception Will throw PipePacketCommunication::no_packets if no packet is available
		 */
		PipeCommPacket &&retrievePacket();

		/*!
		 * \brief Pops available packet from _inPackets
		 * \param inPacketLock Lock for received packets
		 * \exception Will throw PipePacketCommunication::no_packets if no packet is available
		 */
		std::tuple<PipeCommPacket&&, lock_t> retrievePacket(lock_t &&inPacketLock);

	private:
		/*!
		 * \brief Reset Signal Offset (base is SIGRTMIN)
		 */
		uint16_t _confirmSignalOffsetNum;

		/*!
		 * \brief PID of communication partner
		 */
		pid_t _commPID;

		/*!
		 * \brief Packet ID to issue to new packet
		 */
		PipeCommPacket::packet_id_t _nextPackID = 1;

		/*!
		 * \brief Is the server running?
		 */
		bool _commRunning = false;

		/*!
		 * \brief Pipe Communication thread
		 */
		std::thread _commThread;

		/*!
		 * \brief Handle communication via PipeCommunication
		 */
		void commHandler();

		/*!
		 * \brief Lock for _inPackets
		 */
		mutex_t _inPackLock;

		/*!
		 * \brief Received packets
		 */
		std::list<PipeCommPacket> _inPackets;

		/*!
		 * \brief Lock for _outPackets
		 */
		mutex_t _outPackLock;

		/*!
		 * \brief Packets to be sent
		 */
		std::list<PipeCommPacket> _outPackets;

		static inline int getResetSignalNum(uint16_t confirmSignalOffsetNum)
		{	return SIGRTMIN+confirmSignalOffsetNum;	}

		static inline sigset_t generateSigset(int signal)
		{
			sigset_t signalSet;
			sigemptyset(&signalSet);
			sigaddset(&signalSet, signal);

			return signalSet;
		}

		friend PipeCommPacket;
};

#endif // PIPE_PACKET_COMMUNICATION_H
