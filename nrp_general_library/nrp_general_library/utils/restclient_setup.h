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
