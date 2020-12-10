//
// NRP Core - Backend infrastructure to synchronize simulations
//
// Copyright 2020 Michael Zechmair
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This project has received funding from the European Union’s Horizon 2020
// Framework Programme for Research and Innovation under the Specific Grant
// Agreement No. 945539 (Human Brain Project SGA3).
//

#include "nrp_general_library/utils/pipe_communication.h"

#include "nrp_general_library/utils/nrp_exceptions.h"

#include <assert.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

PipeCommunication::PipeCommunication()
{
	if(pipe(this->_pipe) == -1)
		throw NRPException::logCreate("Failed to create pipe");

	int flags = fcntl(this->_pipe[0], F_GETFL, 0);
	fcntl(this->_pipe[0], F_SETFL, flags | O_NONBLOCK);

	flags = fcntl(this->_pipe[1], F_GETFL, 0);
	fcntl(this->_pipe[1], F_SETFL, flags | O_NONBLOCK);
}

PipeCommunication::~PipeCommunication()
{
	this->closeRead();
	this->closeWrite();
}

ssize_t PipeCommunication::readP(void *buf, size_t count, u_int16_t tries, u_int16_t sleepSecs)
{
	assert(this->_pipe[0] >= 0);

	ssize_t totalProcessed = 0;
	while(true)
	{
		const auto processedBytes = read(this->_pipe[0], buf, count);
		if(processedBytes > -1)
		{
			totalProcessed += processedBytes;
			buf = static_cast<char*>(buf) + processedBytes;
		}

		if(static_cast<size_t>(totalProcessed) >= count || --tries == 0)
			return totalProcessed;

		sleep(sleepSecs);
	}
}

ssize_t PipeCommunication::writeP(const void *buf, size_t count, u_int16_t tries, u_int16_t sleepSecs)
{
	assert(this->_pipe[1] >= 0);

	ssize_t totalProcessed = 0;
	while(true)
	{
		auto processedBytes = write(this->_pipe[1], buf, count);
		if(processedBytes == -1)
			return processedBytes;

		totalProcessed += processedBytes;
		if(static_cast<size_t>(totalProcessed) >= count || --tries == 0)
			return totalProcessed;

		sleep(sleepSecs);
	}
}

void PipeCommunication::closeRead()
{
	if(this->_pipe[0] >= 0)
	{
		close(this->_pipe[0]);
		this->_pipe[0] = -1;
	}
}

void PipeCommunication::closeWrite()
{
	if(this->_pipe[1] >= 0)
	{
		close(this->_pipe[1]);
		this->_pipe[1] = -1;
	}
}

int PipeCommunication::readFd() const
{
	return this->_pipe[0];
}

int PipeCommunication::writeFd() const
{
	return this->_pipe[1];
}

