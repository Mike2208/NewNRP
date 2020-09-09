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
