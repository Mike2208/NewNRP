#include "nrp_general_library/utils/file_finder.h"


std::filesystem::path FileFinder::findFile(const std::string &fileName, const std::vector<std::filesystem::path> &searchDirectories)
{
	for(const auto &curDir : searchDirectories)
	{
		const std::filesystem::path curPath = curDir / fileName;
		if(std::filesystem::exists(curPath))
			return curPath;
	}

	return std::filesystem::path("");
}

std::filesystem::path FileFinder::operator()(const std::string &fileName, const std::vector<std::filesystem::path> &searchDirectories)
{	return FileFinder::findFile(fileName, searchDirectories);	}
