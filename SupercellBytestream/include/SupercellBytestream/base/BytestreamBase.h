#pragma once

#include <vector>
#include <cstring>

#include "SupercellBytestream/base/Endian.h"
#include "SupercellBytestream/error/StreamException.h"

namespace sc
{
	class BytestreamBase
	{
	protected:
		virtual size_t _read(void* data, size_t dataSize) = 0;
		virtual size_t _write(void* data, size_t dataSize) = 0;

	public:
		virtual ~BytestreamBase() {};

		virtual uint32_t tell() = 0;
		virtual int set(uint32_t pos) = 0;

		virtual uint32_t size() = 0;
		virtual void* data() = 0;

		virtual void close() = 0;

		bool eof()
		{
			return size() <= tell();
		};

		void skip(uint32_t length)
		{
			if (length + tell() > size())
				set(size());
			else
				set(tell() + length);
		}

		size_t read(void* data, size_t dataSize) {
			if (!closed) {
				return _read(data, dataSize);
			}
			else {
				throw StreamException(StreamError::CLOSED_ERROR, "Failed to read data from closed stream.");
			}
		};

		size_t write(void* data, size_t dataSize) {
			if (!closed) {
				return _write(data, dataSize);
			}
			else {
				throw StreamException(StreamError::CLOSED_ERROR, "Failed to write data to closed stream.");
			}
		};

	protected:
		bool closed = false;
	};
}