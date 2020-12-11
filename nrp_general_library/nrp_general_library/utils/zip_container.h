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

#ifndef ZIP_CONTAINER_H
#define ZIP_CONTAINER_H

#include <filesystem>
#include <string>
#include <vector>
#include <zip.h>

/*!
 * \brief Zip Container Structure. Based on libzip
 */
class ZipContainer
{
		/*!
		 * \brief Wrapper for zip_error_t.
		 * Automatically initializes and cleans up struct
		 */
		struct ZipErrorT : public zip_error_t
		{
			/*!
			 * \brief Constructor. Sets up zip_error_t
			 */
			ZipErrorT();

			/*!
			 * \brief Constructor. Initializes zip_error_t with error code ze
			 * \param ze Error Code
			 */
			ZipErrorT(int ze);

			/*!
			 * \brief Destructor. Cleans up zip_error_t
			 */
			~ZipErrorT();
		};

		/*!
		 * \brief Zip File Wrapper. Automatically closes file descriptor on desctruct
		 */
		struct ZipFileWrapper
		{
			ZipFileWrapper(zip_file_t *zFile);
			~ZipFileWrapper();

			operator zip_file_t*();

			private:
			    zip_file_t *_zFile = nullptr;
		};

		/*!
		 * \brief Buffer size for zip file extraction
		 */
		static constexpr zip_uint64_t BuffCopySize = 1024;

	public:
		/*!
		 * \brief Constructor. Takes a string argument. This is mainly used for Pistache data receiving
		 * \param data Zip File Data. Note: Will use entire data.capacity() as ZIP file array, not just data.size()
		 * \exception Throws std::logic_error on failure
		 */
		ZipContainer(std::string &&data);

		/*!
		 * \brief Constructor. Initializes zip_t
		 * \param data Zip data buffer
		 * \exception Throws std::logic_error on failure
		 */
		ZipContainer(std::vector<uint8_t> &&data);

		/*!
		 * \brief Constructor. Loads data from file at path
		 * \param path Path to Zip Archive
		 * \param readOnly Should archive be opened in read-only mode
		 * \param saveOnDestruct Should the archive be saved automatically on destruct
		 */
		ZipContainer(const std::string &path, bool readOnly, bool saveOnDestruct);

		/*!
		 * \brief Destructor. Will save zip archive if requested
		 */
		~ZipContainer() noexcept;

		/*!
		 * \brief Compress files and directories under path
		 * \param path Path to directory that should be compressed
		 * \param keepRelDirStruct Should the created zip archive keep the relative directory structure to path.
		 * If false, will save files inside path directly
		 * \return Returns ZipContainer with compressed contents
		 * \exception Throws std::logic_error on fail
		 */
		static ZipContainer compressPath(const std::filesystem::path &path, bool keepRelDirStruct = false);

		/*!
		 * \brief Get zip archive's compressed data
		 * \return Returns compressed data
		 */
		std::vector<uint8_t> getCompressedData() const;

		/*!
		 * \brief Extract Zip Files and store them under path
		 * \param path Path to extraction directory
		 * \exception Throws std::logic_error on fail
		 */
		void extractZipFiles(std::string path) const;

		/*!
		 * \brief Save Archive to storage
		 * \param dest File Name
		 * \exception Throws std::logic_error on fail
		 */
		void saveToDestination(const std::string &dest) const;

	private:
		/*!
		 * \brief Zip Buffer
		 */
		zip_t *_data;

		/*!
		 * \brief If true, save Zip Buffer on desctruct
		 */
		bool _saveOnDesctruct = false;

		/*!
		 * \brief Create zip_t* from buffer
		 * \param data Data Buffer
		 * \param length Size of buffer
		 * \return Returns zip_t pointer
		 * \exception Throws std::logic_error on failure
		 */
		static zip_t *createZip(const void *data, zip_uint64_t length);

		/*!
		 * \brief Open a zip archive
		 * \param path Path to archive
		 * \param readOnly Should archive be opened in read-only mode
		 * \return Returns zip_t pointer
		 * \exceptions Throws std::logic_error on failure
		 */
		static zip_t *openZipArchive(const std::string &path, bool readOnly);

		/*!
		 * \brief Add all files of zip archive src to dest
		 * \param dest Destination Zip Archive
		 * \param src Source Zip Archive
		 * \exceptions Throws std::logic_error on failure
		 */
		static void addZipToZip(zip_t *dest, zip_t *src);

		ZipContainer(zip_t *data, bool saveOnDestruct);
};

#endif // ZIP_CONTAINER_H
