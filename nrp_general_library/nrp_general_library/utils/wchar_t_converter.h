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

#ifndef WCHAR_T_CONVERTER_H
#define WCHAR_T_CONVERTER_H

#include <vector>
#include <string>

/*!
 * \brief Converts and stores an char*[] to wchar*[].
 * Used mainly to convert argv from char*[] to wchar*[] for Python Initialization
 */
class WCharTConverter
{
	public:
		WCharTConverter(int argc, const char *const *argv);

		wchar_t **getWCharTPointers();
		int getWCharSize() const;

	private:
		std::vector<std::wstring> _wCharTContainer;
		std::vector<wchar_t*> _wCharTPointer;
};

#endif // WCHAR_T_CONVERTER_H
