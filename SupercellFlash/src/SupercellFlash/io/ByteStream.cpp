#include "SupercellFlash/io/ByteStream.h"

namespace sc
{
	ByteStream::ByteStream(const std::vector<uint8_t>& data)
	{
		m_data = data;
	}

	ByteStream::ByteStream(FILE* file)
	{
		m_file = file;
	}

	ByteStream::~ByteStream()
	{
		if (m_file != nullptr)
		{
			fclose(m_file);
			m_file = nullptr;
		}

		m_data.clear();
		m_offset = 0;
	}

	uint32_t ByteStream::tell() const
	{
		if (m_file != nullptr)
		{
			return static_cast<uint32_t>(ftell(m_file));
		}
		return m_offset;
	}

	void ByteStream::seek(uint32_t offset)
	{
		if (m_file != nullptr)
		{
			fseek(m_file, offset, SEEK_SET);
		}

		if (size() >= offset)
		{
			m_offset = offset;
		}
	}

	std::size_t ByteStream::size()
	{
		if (m_file != nullptr)
		{
			long orig_pos = ftell(m_file);

			fseek(m_file, 0, SEEK_END);
			uint32_t size = static_cast<uint32_t>(ftell(m_file));
			fseek(m_file, orig_pos, SEEK_SET);

			return size;
		}
		return m_data.size() - m_offset;
	}

	void ByteStream::skip(int32_t length)
	{
		if (m_file != nullptr)
		{
			fseek(m_file, tell() + length, SEEK_SET);
		}
		m_offset += length;
	}

	bool ByteStream::readBool()
	{
		return (readUnsignedChar() == 1);
	}

	int8_t ByteStream::readChar()
	{
		int8_t result;
		read(&result, sizeof(result));
		return result;
	}

	uint8_t ByteStream::readUnsignedChar()
	{
		uint8_t result;
		read(&result, sizeof(result));
		return result;
	}

	int16_t ByteStream::readShort()
	{
		int16_t result;
		read(&result, sizeof(result));
		return result;
	}

	uint16_t ByteStream::readUnsignedShort()
	{
		uint16_t result;
		read(&result, sizeof(result));
		return result;
	}

	int32_t ByteStream::readInt()
	{
		int32_t result;
		read(&result, sizeof(result));
		return result;
	}

	float ByteStream::readTwip()
	{
		return (float)readInt() * 0.05f;
	}

	std::string ByteStream::readAscii()
	{
		uint8_t length = readUnsignedChar();
		if (length == 0xFF)
			return "";

		char* str = new char[length]();
		read(str, length);

		return std::string(str, length);
	}

	std::vector<int8_t> ByteStream::readByteArray(uint32_t count)
	{
		std::vector<int8_t> result;

		for (uint32_t i = 0; i < count; i++)
			result.push_back(readChar());

		return result;
	}

	std::vector<int16_t> ByteStream::readShortArray(uint32_t count)
	{
		std::vector<int16_t> result;

		for (uint32_t i = 0; i < count; i++)
			result.push_back(readShort());

		return result;
	}

	std::vector<int32_t> ByteStream::readIntArray(uint32_t count)
	{
		std::vector<int32_t> result;

		for (uint32_t i = 0; i < count; i++)
			result.push_back(readInt());

		return result;
	}

	void ByteStream::writeBool(bool value)
	{
		writeUnsignedChar((uint8_t)value);
	}

	void ByteStream::writeChar(int8_t value)
	{
		write(&value, sizeof(value));
	}

	void ByteStream::writeUnsignedChar(uint8_t value)
	{
		write(&value, sizeof(value));
	}

	void ByteStream::writeShort(int16_t value)
	{
		write(&value, sizeof(value));
	}

	void ByteStream::writeUnsignedShort(uint16_t value)
	{
		write(&value, sizeof(value));
	}

	void ByteStream::writeInt(int32_t value)
	{
		write(&value, sizeof(value));
	}

	void ByteStream::writeTwip(float value)
	{
		writeInt((int32_t)(value * 20.0f));
	}

	void ByteStream::writeAscii(const std::string& string)
	{
		uint8_t size = (uint8_t)string.size();

		if (size == 0)
			size = 0xFF;

		writeUnsignedChar(size);

		if (size != 0xFF)
		{
			for (uint8_t i = 0; i < size; i++)
				writeChar(string[i]);
		}
	}

	void ByteStream::writeByteArray(const std::vector<int8_t>& arr)
	{
		for (uint32_t i = 0; i < arr.size(); i++)
			writeChar(arr[i]);
	}

	void ByteStream::writeShortArray(const std::vector<int16_t>& arr)
	{
		for (int32_t i = 0; i < arr.size(); i++)
			writeShort(arr[i]);
	}

	void ByteStream::writeIntArray(const std::vector<int32_t>& arr)
	{
		for (int32_t i = 0; i < arr.size(); i++)
			writeInt(arr[i]);
	}

	std::size_t ByteStream::read(void* dst, std::size_t length)
	{
		if (m_file)
		{
			std::size_t toRead = (tell() + length) > size() ? size() - tell() : length;
			return fread(dst, 1, toRead, m_file);
		}
		else
		{
			if (length == 0 || m_offset >= size())
			{
				return 0;
			}

			size_t toRead = size() - m_offset;
			if (toRead > length)
			{
				toRead = length;
			}

			std::memcpy(dst, m_data.data() + m_offset, toRead);

			m_offset += toRead;

			return toRead;
		}
	}

	std::size_t ByteStream::write(void* src, std::size_t length)
	{
		if (m_file != nullptr)
		{
			std::size_t result = fwrite(src, 1, length, m_file);
			m_fileSize += length;

			return result;
		}
		else
		{
			auto oldSize = m_data.size();
			m_data.resize(oldSize + length);
			std::memcpy(&m_data[oldSize], src, length);

			m_offset += length;

			return length;
		}
	}

}
