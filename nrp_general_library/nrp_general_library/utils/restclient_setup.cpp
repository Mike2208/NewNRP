#include "nrp_general_library/utils/restclient_setup.h"

std::unique_ptr<RestClientSetup> RestClientSetup::_instance = std::unique_ptr<RestClientSetup>(new RestClientSetup());

RestClientSetup::~RestClientSetup()
{	RestClient::disable();	}

RestClientSetup *RestClientSetup::getInstance()
{	return RestClientSetup::_instance.get();	}

RestClientSetup *RestClientSetup::resetInstance()
{
	RestClientSetup::_instance.reset(new RestClientSetup());
	return RestClientSetup::getInstance();
}

RestClientSetup *RestClientSetup::ensureInstance()
{
	auto *const pInstance = RestClientSetup::getInstance();
	if(pInstance != nullptr)
		return pInstance;

	return RestClientSetup::resetInstance();
}

RestClientSetup::RestClientSetup()
{	RestClient::init();	}
