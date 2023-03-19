#pragma once

#include "SupercellBytestream/base/Bytestream.h"
#include "SupercellBytestream/error/StreamException.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace sc {
	class FileStream : public Bytestream {
	protected:
		FILE* file;
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

		int set(uint32_t pos) override
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
			set(pos);
			return data;
		}

		void close() override {
			fclose(file);
		};
	};

	class WriteFileStream : public FileStream {
	public:
		WriteFileStream(std::string filepath) {
			file = fopen(filepath.c_str(), "wb");
			fileInitialize();
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
				fileInitialize();
			}
			else {
				throw StreamException(StreamError::EXIST_ERROR, "File not found: " + filepath);
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
	};
}