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

#ifndef RESTCLIENT_SETUP_H
#define RESTCLIENT_SETUP_H

#include <memory>
#include <restclient-cpp/connection.h>
#include <restclient-cpp/restclient.h>

/*!
 * \brief Singleton. Class to setup RestClient and initialize features such as timeouts, authentications, ...
 */
class RestClientSetup
{
	public:
		~RestClientSetup();

		// Delete move/copy constructors+operators for singleton
		RestClientSetup(const RestClientSetup&) = delete;
		RestClientSetup(RestClientSetup&&) = delete;

		RestClientSetup &operator=(const RestClientSetup&) = delete;
		RestClientSetup &operator=(RestClientSetup&&) = delete;

		/*!
		 * \brief Get RestClientSetup instance. Returns nullptr if not yet initialized
		 */
		static RestClientSetup *getInstance();

		/*!
		 * \brief Resets RestClientSetup
		 */
		static RestClientSetup *resetInstance();

		/*!
		 * \brief Ensure that RestClientSetup has been initialized
		 */
		static RestClientSetup *ensureInstance();

	private:
		static std::unique_ptr<RestClientSetup> _instance;

		RestClientSetup();
};

#endif // RESTCLIENT_SETUP_H
