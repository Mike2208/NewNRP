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

#ifndef FIXED_STRING_H
#define FIXED_STRING_H

#include <string>
#include <string_view>

template<std::size_t N>
struct FixedString
{
		static constexpr auto Length = N;

		constexpr FixedString(const char(&str)[N])
		{
			for(unsigned i = 0; i != N; ++i)
				m_data[i] = str[i];
		}

		constexpr FixedString(const FixedString<N> &str) = default;

		template<class T>
		constexpr FixedString(T str)
		    : FixedString(str.m_data)
		{
			static_assert(N==T::Length, "String mismatch");
		}

		constexpr FixedString()
		{}

		//auto operator<=>(const basic_fixed_string &, const basic_fixed_string &) = default;

		char m_data[N]{};
		constexpr operator auto() const
		{ return m_data; }

		/*!
		 * \brief This is only here so that the CLang LSP doesn't complain about template parameter deduction. Remove once this is implemented
		 */
		constexpr operator int() const
		{ return Length;	}

		constexpr operator std::string_view() const
		{ return std::string_view(m_data); }

		constexpr operator std::string() const
		{ return std::string(m_data); }

		constexpr const char *data() const
		{ return m_data; }

		template<std::size_t M>
		constexpr bool compare(const char(&other)[M]) const
		{
			const auto l = N<M ? N : M;
			for(std::size_t i = 0; i < l; ++i)
			{
				const auto cmp = other[i] - m_data[i];
				if(cmp)
					return cmp;
			}

			return M-N;
		}

		constexpr bool compare(const char *const str) const
		{
			std::size_t i = 0;
			for(; i < N; ++i)
			{
				const auto cmp = str[i] - m_data[i];
				if(cmp)
					return cmp;

				if(str == 0)
					break;
			}

			return i-N;
		}

		constexpr bool compare(const std::string_view &str) const
		{	return this->compare(str.data());	}
};

template<std::size_t N>
FixedString(const char (&str)[N]) -> FixedString<N>;

template<std::size_t N>
FixedString(const FixedString<N> &str) -> FixedString<N>;

FixedString() -> FixedString<0>;

template<class T>
FixedString(T str) -> FixedString<T::Length>;

#endif // FIXED_STRING_H
