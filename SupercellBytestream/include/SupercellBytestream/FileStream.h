#pragma once

#include "SupercellBytestream/base/Bytestream.h"
#include "SupercellBytestream/error/StreamException.h"
#include "SupercellBytestream/error/FileExistException.h"

#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

using std::ifstream;
using std::ofstream;

namespace sc {
	/*class FileStream : public Bytestream {
	protected:
		FILE* file = NULL;
		uint32_t fileSize = 0;

		void fileInitialize() {
			fseek(file, 0, SEEK_END);
			fileSize = static_cast<uint32_t>(ftell(file));
			fseek(file, 0, SEEK_SET);
		};

		size_t _read(void* buff, size_t buffSize) override
		{
			throw StreamException(StreamError::READ_ERROR, "Failed to read data from incorrectly initialized file.");
		};

		size_t _write(void* buff, size_t buffSize) override
		{
			throw StreamException(StreamError::WRITE_ERROR, "Failed to write data to incorrectly initialized file.");
		};

	public:
		uint32_t tell() override
		{
			return static_cast<uint32_t>(ftell(file));
		};

		int seek(uint32_t pos) override
		{
			return fseek(file, pos, SEEK_SET);
		};

		uint32_t size() override
		{
			return fileSize;
		};
		void* data() override
		{
			uint32_t pos = tell();
			void* data = malloc(size());
			read(data, size());
			seek(pos);
			return data;
		}

		void close() override {
			fclose(file);
		};
	};*/

	class WriteFileStream: public Bytestream {
	private:
		ofstream file;
		uint32_t fileSize = 0;

	public:
		WriteFileStream(std::string filepath) {
			file.open(filepath.c_str(), std::ios_base::binary);

			file.seekp(0, std::ios::end);
			fileSize = static_cast<uint32_t>(file.tellp());
			file.seekp(0);
		}

		size_t _read(void* data, size_t dataSize) override
		{
			return 0;
		}

		size_t _write(void* buff, size_t buffSize)
		{
			size_t pos = file.tellp();

			file.write((char*)buff, buffSize);

			const size_t result = (size_t)file.tellp() - pos;
			fileSize += static_cast<uint32_t>(result);
			return result;
		}

		uint32_t tell()
		{
			return static_cast<uint32_t>(file.tellp());
		}

		void seek(uint32_t position)
		{
			file.seekp(position);
		}

		uint32_t size()
		{
			return fileSize;
		}

		void close() {
			file.close();
			closed = true;
		}
	};

	class ReadFileStream : public Bytestream {
	private:
		ifstream file;
		uint32_t fileSize = 0;

	public:
		ReadFileStream(std::string filepath) {
			if (fs::exists(filepath)) {
				file.open(filepath.c_str(), std::ios_base::binary);

				file.seekg(0, std::ios::end);
				fileSize = static_cast<uint32_t>(file.tellg());
				file.seekg(0);
			}
			else {
				throw FileExistException(filepath);
			}
		}

		size_t _read(void* buff, size_t buffSize)
		{
			size_t toRead = (tell() + buffSize) > size() ? size() - tell() : buffSize;

			file.read((char*)buff, toRead);

			return toRead;
		};

		size_t _write(void* data, size_t dataSize) {
			return 0;
		}

		uint32_t tell()
		{
			return static_cast<uint32_t>(file.tellg());
		}

		void seek(uint32_t position)
		{
			file.seekg(position);
		}

		uint32_t size()
		{
			return fileSize;
		}

		void close() {
			file.close();
			closed = true;
		}
	};

	/*class FileStream : public Bytestream {
	protected:
		FILE* file = NULL;
		uint32_t fileSize = 0;

		size_t _read(void* buff, size_t buffSize) override
		{
			throw StreamException(StreamError::READ_ERROR, "Failed to read data from incorrectly initialized file.");
		};

		size_t _write(void* buff, size_t buffSize) override
		{
			throw StreamException(StreamError::WRITE_ERROR, "Failed to write data to incorrectly initialized file.");
		};

	public:
		uint32_t tell() override
		{
			return static_cast<uint32_t>(ftell(file));
		};

		void seek(uint32_t pos) override
		{
			fseek(file, pos, SEEK_SET);
		};

		uint32_t size() override
		{
			return fileSize;
		};

		void close() override {
			fclose(file);
		};
	};

	class WriteFileStream : public FileStream {
	public:
		WriteFileStream(std::string filepath) {
			file = fopen(filepath.c_str(), "wb");
		}
		size_t _write(void* buff, size_t buffSize) override
		{
			size_t result = fwrite(
				buff,
				1,
				buffSize,
				file
			);
			fileSize += static_cast<uint32_t>(result);
			return result;
		};
	};

	class ReadFileStream : public FileStream {
	public:
		ReadFileStream(std::string filepath) {
			if (fs::exists(filepath)) {
				file = fopen(filepath.c_str(), "rb");

				fseek(file, 0, SEEK_END);
				fileSize = static_cast<uint32_t>(ftell(file));
				fseek(file, 0, SEEK_SET);
			}
			else {
				throw FileExistException(filepath);
			}
		}

		size_t _read(void* buff, size_t buffSize) override
		{
			size_t toRead = (tell() + buffSize) > size() ? size() - tell() : buffSize;
			return fread(
				buff,
				1,
				toRead,
				file
			);
		};
	};*/
}