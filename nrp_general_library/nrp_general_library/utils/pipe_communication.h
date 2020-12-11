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

#ifndef PIPE_COMMUNICATION_H
#define PIPE_COMMUNICATION_H

#include <sys/types.h>

/*!
 * \brief Creates a pipe, used for inter-process communication.
 * Call fork after creating a PipeCommunication object
 */
class PipeCommunication
{
	public:
		PipeCommunication();
		~PipeCommunication();

		/*!
		 * \brief Read from pipe
		 * \param buf buffer to read to
		 * \param count Number of bytes to read
		 * \param tries How often to try to read from buffer
		 * \param sleepSecs How long to wait between retries
		 * \return -1 on error, else number of read bytes
		 */
		ssize_t readP(void *buf, size_t count, u_int16_t tries = 1, u_int16_t sleepSecs = 0);

		/*!
		 * \brief Write to pipe
		 * \param buf buffer to write from
		 * \param count Number of bytes to write
		 * \param tries How often to try to write from buffer
		 * \param sleepSecs How long to wait between retries
		 * \return -1 on error, else number of written bytes
		 */
		ssize_t writeP(const void *buf, size_t count, u_int16_t tries = 1, u_int16_t sleepSecs = 0);

		/*!
		 * \brief Close Read direction
		 */
		void closeRead();

		/*!
		 * \brief Close Write direction
		 */
		void closeWrite();

		/*!
		 * \brief Get Read File Descriptor
		 */
		int readFd() const;

		/*!
		 * \brief Get Write File Descriptor
		 */
		int writeFd() const;

	private:
		/*!
		 * \brief Pipe File Descriptors
		 */
		int _pipe[2] = {-1, -1};

};

#endif
