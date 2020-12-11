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

#ifndef PTR_TEMPLATES_H
#define PTR_TEMPLATES_H

#include <memory>

template<class T>
class PtrTemplates
{
	public:
		using shared_ptr = std::shared_ptr<T>;
		using const_shared_ptr = std::shared_ptr<const T>;

		using unique_ptr = std::unique_ptr<T>;
		using const_unique_ptr = std::unique_ptr<const T>;

		constexpr auto operator<=>(const PtrTemplates<T>&) const = default;
};

#endif
