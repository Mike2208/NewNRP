#include "nrp_general_library/utils/wchar_t_converter.h"

#include "nrp_general_library/utils/nrp_exceptions.h"

#include <assert.h>
#include <iostream>
#include <stdexcept>
#include <stdlib.h>
#include <string.h>

WCharTConverter::WCharTConverter(int argc, const char *const *argv)
{
	this->_wCharTContainer.resize(argc);
	for(int i=0; i < argc; ++i)
	{
		// Setup conversion
		const char *curString = argv[i];
		auto &curWString = this->_wCharTContainer[i];

		// Get size of converted wstring
		std::mbstate_t ps;
		memset(&ps, 0, sizeof(mbstate_t));		// Set ps to initial state
		const auto wstrLen = mbsrtowcs(nullptr, &curString, 0, &ps);

		if(wstrLen == static_cast<size_t>(-1))
			throw NRPException::logCreate("Could not convert argument " + std::to_string(i) + " \"" + curString + "\" to wstring");

		// Reserve enough space in wstring
		curWString.resize(wstrLen+1);

		// Convert string
		curString = argv[i];
		mbsrtowcs(&(curWString[0]), &curString, curWString.size()+1, &ps);

		curWString.shrink_to_fit();
	}

	this->_wCharTPointer.reserve(this->_wCharTContainer.size());
	for(auto &curWString : this->_wCharTContainer)
	{
		this->_wCharTPointer.push_back(&curWString[0]);
	}
}

wchar_t **WCharTConverter::getWCharTPointers()
{
	if(!this->_wCharTPointer.empty())
		return &this->_wCharTPointer[0];
	else
		return nullptr;
}

int WCharTConverter::getWCharSize() const
{
	return static_cast<int>(this->_wCharTPointer.size());
}
