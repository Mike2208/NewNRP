#include "nrp_general_library/transceiver_function/transceiver_function_sorted_results.h"

void TransceiverFunctionSortedResults::addResults(const TransceiverFunctionManager::tf_results_t &results)
{
	for(const auto &cR : results)
	{
		this->_pyList.push_back(cR.DeviceList);

		for(DeviceInterface *const pInt : cR.Devices)
			this->addResult(pInt);
	}
}

TransceiverFunctionSortedResults TransceiverFunctionSortedResults::sortResults(const TransceiverFunctionManager::tf_results_t &results)
{
	return TransceiverFunctionSortedResults().sortResults(results);
}

TransceiverFunctionSortedResults::interface_results_t::iterator TransceiverFunctionSortedResults::addResult(DeviceInterface *result)
{
	auto mapIterator = this->find(result->engineName());
	if(mapIterator == this->end())
	{
		// Create new interface map if not yet available
		auto emplaceRes = this->emplace(result->engineName(), devices_t({result}));
		// TODO: Check if emplace was successful (emplace.second)

		mapIterator = emplaceRes.first;
	}
	else
		mapIterator->second.push_back(result);

	return mapIterator;
}
