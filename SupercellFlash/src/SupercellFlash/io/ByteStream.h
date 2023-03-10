#pragma once

#include <vector>
#include <string>
#include <cstdio>

namespace sc
{
	class ByteStream
	{
	public:
		ByteStream() = default;
		ByteStream(const std::vector<uint8_t>& data);
		ByteStream(FILE* file);

		virtual ~ByteStream();

	public:
		std::size_t read(void* dst, std::size_t length);
		std::size_t write(void* src, std::size_t length);

		uint32_t tell() const;
		void seek(uint32_t offset);

		std::size_t size();

		void skip(int32_t length);

	public:
		bool readBool();

		int8_t readChar();
		uint8_t readUnsignedChar();

		int16_t readShort();
		uint16_t readUnsignedShort();

		int32_t readInt();

		float readTwip();

		std::string readAscii();

		std::vector<int8_t> readByteArray(uint32_t count);
		std::vector<int16_t> readShortArray(uint32_t count);
		std::vector<int32_t> readIntArray(uint32_t count);

	public:
		void writeBool(bool value);

		void writeChar(int8_t value);
		void writeUnsignedChar(uint8_t value);

		void writeShort(int16_t value);
		void writeUnsignedShort(uint16_t value);

		void writeInt(int32_t value);

		void writeTwip(float value);

		void writeAscii(const std::string& string);

		void writeByteArray(const std::vector<int8_t>& arr);
		void writeShortArray(const std::vector<int16_t>& arr);
		void writeIntArray(const std::vector<int32_t>& arr);

	private:
		std::vector<uint8_t> m_data;
		std::size_t m_offset = 0;

		FILE* m_file = nullptr;
		uint32_t m_fileSize = 0;
	};
}
