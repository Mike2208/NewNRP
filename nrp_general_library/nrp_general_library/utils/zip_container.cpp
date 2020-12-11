//
// NRP Core - Backend infrastructure to synchronize simulations
//
// Copyright 2020 Michael Zechmair
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// This project has received funding from the European Union’s Horizon 2020
// Framework Programme for Research and Innovation under the Specific Grant
// Agreement No. 945539 (Human Brain Project SGA3).
//

#include "nrp_general_library/utils/zip_container.h"

#include "nrp_general_library/utils/nrp_exceptions.h"

#include <assert.h>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string.h>

struct ZipSourceWrapper
{
	ZipSourceWrapper(zip_source_t *zip_source)
	    : _zip_source(zip_source)
	{}

	~ZipSourceWrapper()
	{	zip_source_free(this->_zip_source);	}

	operator zip_source_t*() const
	{	return this->_zip_source;	}

	private:
	    zip_source_t *_zip_source;
};

struct ZipWrapper
{
	ZipWrapper(zip_t *zip)
	    : _zip(zip)
	{}

	~ZipWrapper()
	{
		if(this->_zip != nullptr)
			zip_discard(this->_zip);
	}

	zip_t *release()
	{
		zip_t *const retVal = this->_zip;
		this->_zip = nullptr;
		return retVal;
	}

	void closeAndSaveZip()
	{
		if(zip_close(this->_zip) != 0)
			throw NRPException::logCreate(std::string("Could not save Zip Container: ") + zip_strerror(this->_zip));

		this->_zip = nullptr;
	}

	operator zip_t*() const
	{	return this->_zip;	}

	private:
	    zip_t *_zip;
};

ZipContainer::ZipErrorT::ZipErrorT()
{	zip_error_init(this);	}

ZipContainer::ZipErrorT::ZipErrorT(int ze)
{	zip_error_init_with_code(this, ze);	}

ZipContainer::ZipErrorT::~ZipErrorT()
{	zip_error_fini(this);	}


ZipContainer::ZipFileWrapper::ZipFileWrapper(zip_file_t *zFile)
    : _zFile(zFile)
{}

ZipContainer::ZipFileWrapper::~ZipFileWrapper()
{
	if(this->_zFile != nullptr)
	{
		const auto errCode = zip_fclose(this->_zFile);
		if(errCode != 0)
		{
			ZipErrorT zErr(errCode);
			NRPLogger::SPDErrLogDefault(std::string("Zip File could not be closed: ") + zip_error_strerror(&zErr));
		}
	}
}

ZipContainer::ZipFileWrapper::operator zip_file_t *()
{	return this->_zFile;	}


ZipContainer::ZipContainer(std::string &&data)
    : _data(ZipContainer::createZip(data.data(), data.capacity()))
{}

ZipContainer::ZipContainer(std::vector<uint8_t> &&data)
    : _data(ZipContainer::createZip(data.data(), data.size()))
{}

ZipContainer::ZipContainer(const std::string &path, bool readOnly, bool saveOnDestruct)
    : _data(ZipContainer::openZipArchive(path, readOnly)),
      _saveOnDesctruct(saveOnDestruct)
{}

ZipContainer::~ZipContainer() noexcept
{
	if(this->_data != nullptr)
	{
		if(this->_saveOnDesctruct)
		{
			const auto zErr = zip_close(this->_data);
			if(zErr != 0)
			{
				NRPLogger::SPDErrLogDefault("Could not save Zip Container");

				zip_discard(this->_data);
			}
		}
		else
			zip_discard(this->_data);

		this->_data = nullptr;
	}

	this->_saveOnDesctruct = false;
}

ZipContainer ZipContainer::compressPath(const std::filesystem::path &path, bool keepRelDirStruct)
{
	namespace fs = std::filesystem;

	ZipErrorT zErr;

	zip_source_t *pZSource = zip_source_buffer_create(nullptr, 0, 0, &zErr);
	ZipWrapper pZArch = zip_open_from_source(pZSource, ZIP_TRUNCATE, &zErr);
	if(pZArch == nullptr)
		throw NRPException::logCreate(std::string("Failed to open temporary file for archive: ") + zip_error_strerror(&zErr));

	// Set string of directory structure that should be removed
	for(const auto &f : fs::recursive_directory_iterator(path, fs::directory_options::follow_directory_symlink))
	{
		const std::string fName = keepRelDirStruct ? f.path() : fs::relative(f.path(), path);

		if(f.is_directory())
		{
			if(zip_dir_add(pZArch, fName.c_str(), ZIP_FL_ENC_GUESS) != 0)
				throw NRPException::logCreate("Failed to add directory \"" + fName + "\" to zip archive: " + zip_strerror(pZArch));
		}
		else if(f.is_regular_file())
		{
			ZipSourceWrapper pZSource = zip_source_file(pZArch, f.path().c_str(), 0, 0);
			if(pZSource == nullptr)
				throw NRPException::logCreate("Failed to add file \"" + fName + "\" to zip archive: " + zip_strerror(pZArch));

			if(zip_file_add(pZArch, fName.c_str(), pZSource, ZIP_FL_ENC_GUESS) < 0)
				throw NRPException::logCreate("Failed to add file \"" + fName + "\" to zip archive: " + zip_strerror(pZArch));
		}
	}

	return ZipContainer(pZArch.release(), false);
}

std::vector<uint8_t> ZipContainer::getCompressedData() const
{
	ZipErrorT zErr;

	// Create new zip archive with pZSource as target buffer
	ZipSourceWrapper pZSource = zip_source_buffer_create(nullptr, 0, 0, &zErr);
	zip_source_keep(pZSource);

	ZipWrapper pZArch = zip_open_from_source(pZSource, ZIP_TRUNCATE, &zErr);
	if(pZArch == nullptr)
		throw NRPException::logCreate(std::string("Failed to open temporary file for archive: ") + zip_error_strerror(&zErr));

	// Add _data to new zip archive
	ZipContainer::addZipToZip(pZArch, this->_data);

	// Compress data
	pZArch.closeAndSaveZip();

	// Get compressed data size
	zip_source_open(pZSource);
	zip_source_seek(pZSource, 0, SEEK_END);
	const auto zSize = zip_source_tell(pZSource);
	zip_source_seek(pZSource, 0, SEEK_SET);

	// Store compressed data
	std::vector<uint8_t> retVal(zSize);
	zip_source_read(pZSource, retVal.data(), zSize);

	return retVal;
}

void ZipContainer::extractZipFiles(std::string path) const
{
	if(path.back() != '/')
		path += '/';

	auto *const pZip = this->_data;

	const zip_int64_t numIndices = zip_get_num_entries(pZip, 0);
	if(numIndices <= 0)
		return;

	uint8_t buffer[ZipContainer::BuffCopySize];

	// Iterate over all files in archive
	for(zip_int64_t cI = 0; cI < numIndices; ++cI)
	{
		// Get File Stats
		struct zip_stat zStat;
		if(zip_stat_index(pZip, cI, 0, &zStat) != 0)
			throw NRPException::logCreate(std::string("Failed to read file stats from Zip Archive: ") + zip_strerror(pZip));

		const auto nLen = strlen(zStat.name);
		if(zStat.name[nLen-1] == '/')
		{
			// Create directory
			std::filesystem::create_directory(path+zStat.name);
		}
		else
		{
			// Create file
			// Get File Descriptor for reading file
			ZipFileWrapper zFile = zip_fopen_index(pZip, cI, 0);
			if(zFile == nullptr)
				throw NRPException::logCreate(std::string("Failed to read file from Zip Archive: ") + zip_strerror(pZip));

			// Create File
			std::fstream file(path+zStat.name, std::ios::out | std::ios::binary | std::ios::trunc);

			// Extract file data from archive
			zip_uint64_t remSize = zStat.size;
			while(remSize > 0)
			{
				const auto copySize = std::min(remSize, ZipContainer::BuffCopySize);

				// Extract file data
				if(zip_fread(zFile, buffer, copySize) != 0)
					throw NRPException::logCreate(std::string("Failure while reading zip file: ") + zStat.name);

				// Write file data
				file.write(reinterpret_cast<const char*>(buffer), copySize);
				if(file.fail())
					throw NRPException::logCreate(std::string("Failure while extracting zip file: ") + zStat.name);

				remSize -= copySize;
			}
		}
	}
}

void ZipContainer::saveToDestination(const std::string &dest) const
{
	int cErr;

	// Open new zip file at dest
	ZipWrapper pZArch =	zip_open(dest.c_str(), ZIP_CREATE | ZIP_TRUNCATE, &cErr);
	if(pZArch == nullptr)
	{
		ZipErrorT zErr(cErr);
		throw NRPException::logCreate("Failed to create empty zip archive at \"" + dest + "\": " + zip_error_strerror(&zErr));
	}

	ZipContainer::addZipToZip(pZArch, this->_data);

	// Save data to dest and close zip file
	pZArch.closeAndSaveZip();
}

zip_t *ZipContainer::createZip(const void *data, zip_uint64_t length)
{
	ZipErrorT zErr;
	zip_source_t *pZipSource = zip_source_buffer_create(0, 0, 0, &zErr);
	if(pZipSource == nullptr)
		throw NRPException::logCreate(std::string("Failed to create Zip buffer from data: ") + zip_error_strerror(&zErr));

	// Copy data to buffer managed by ZipContainer
	if(int cErr = zip_source_begin_write(pZipSource) < 0 ||
	        zip_source_write(pZipSource, data, length) < 0 ||
	        zip_source_commit_write(pZipSource) < 0)
	{
		zErr = ZipErrorT(cErr);
		const std::string zErrMsg = cErr == 0 ? zip_error_strerror(zip_source_error(pZipSource)) : zip_error_strerror(&zErr);

		throw NRPException::logCreate("Failed to copy Zip data to buffer: " + zErrMsg);
	}

	// Create zip struct. Will take ownership of pZipSource and delete on close
	zip_t *pZip = zip_open_from_source(pZipSource, 0, &zErr);
	if(pZip == nullptr)
		throw NRPException::logCreate(std::string("Error while reading Zip buffer data: ") + zip_error_strerror(&zErr));

	return pZip;
}

zip_t *ZipContainer::openZipArchive(const std::string &path, bool readOnly)
{
	int cErr;

	const int flags = readOnly ? ZIP_RDONLY : 0;
	zip_t *const retVal = zip_open(path.data(), flags, &cErr);
	if(retVal == nullptr)
	{
		ZipErrorT zErr(cErr);
		throw NRPException::logCreate("Failed to open zip archive at \"" + path + "\": " + zip_error_strerror(&zErr));
	}

	return retVal;
}

void ZipContainer::addZipToZip(zip_t *dest, zip_t *src)
{
	assert(dest != nullptr && src != nullptr);

	const auto numFiles = zip_get_num_files(src);
	if(numFiles <= 0)
	{
		assert(numFiles == 0);
		return;
	}

	// Iteratively add all files of existing _data zip archive to new one
	for(int cI = 0; cI < numFiles; ++cI)
	{
		zip_source_t *pZSource = zip_source_zip(dest, src, cI, 0, 0, -1);
		if(pZSource == nullptr)
			throw NRPException::logCreate(std::string("Failed to save zip archive: ") + zip_strerror(dest));
	}
}

ZipContainer::ZipContainer(zip_t *data, bool saveOnDestruct)
    : _data(data),
      _saveOnDesctruct(saveOnDestruct)
{}
