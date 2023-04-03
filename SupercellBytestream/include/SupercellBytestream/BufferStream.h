#pragma once

#include "SupercellBytestream/base/Bytestream.h"

namespace sc {
	class BufferStream : public Bytestream {
	private:
		std::vector<uint8_t>* m_buffer = nullptr;
		size_t position = 0;

	public:
		explicit BufferStream(std::vector<uint8_t>* buffer) : m_buffer(buffer) {}

		size_t _read(void* data, size_t dataSize) override
		{
			if (dataSize == 0 || position >= size())
			{
				return 0;
			}

			size_t toRead = size() - position;
			if (toRead > dataSize)
			{
				toRead = dataSize;
			}

			std::memcpy(data, m_buffer->data() + position, toRead);

			position += toRead;
			return toRead;
		};

		size_t _write(void* data, size_t dataSize) override
		{
			auto oldSize = m_buffer->size();
			m_buffer->resize(oldSize + dataSize);
			std::memcpy(&(*m_buffer)[oldSize], data, dataSize);

			position += dataSize;

			return dataSize;
		};

		uint32_t tell() override
		{
			return static_cast<uint32_t>(position);
		};

		void seek(uint32_t pos) override
		{
			if (size() >= pos)
			{
				position = pos;
			}
			else
			{
				position = size();
			}
		};

		uint32_t size() override
		{
			return static_cast<uint32_t>(m_buffer->size());
		};

		void* data()
		{
			return m_buffer->data();
		}

		void close() override
		{
			m_buffer = nullptr;
			position = 0;
		};

		std::vector<uint8_t>* buffer(uint8_t* data) {
			return m_buffer;
		}
	};
}