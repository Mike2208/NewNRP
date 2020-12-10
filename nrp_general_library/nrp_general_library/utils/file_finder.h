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

#ifndef FILE_FINDER_H
#define FILE_FINDER_H

#include <filesystem>
#include <fstream>
#include <vector>

/*!
 * \brief Find a file in a list of directories
 */
class FileFinder
{
	public:
		/*!
		 * \brief Find first instance of file in searchDirectories
		 * \param fileName Filename to find
		 * \param searchDirectories Directories under which to search for file
		 * \return Returns Path to fileName. Empty if not found
		 */
		static std::filesystem::path findFile(const std::string &fileName, const std::vector<std::filesystem::path> &searchDirectories);

		/*!
		 * \brief Find first instance of file in searchDirectories
		 * \param fileName Filename to find
		 * \param searchDirectories Directories under which to search for file
		 * \return Returns Path to fileName. Empty if not found
		 */
		std::filesystem::path operator() (const std::string &fileName, const std::vector<std::filesystem::path> &searchDirectories);
};

#endif // FILE_FINDER_H
