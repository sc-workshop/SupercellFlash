#pragma once 

#include "BytestreamBase.h"

namespace sc {
	class Bytestream : public BytestreamBase {
		/* Read/Write functions for integers */

		/* 8-bit integer */
	public:
		uint8_t readUInt8()
		{
			uint8_t data;
			size_t readRes = read(&data, sizeof(uint8_t));

			if (sizeof(uint8_t) == readRes)
			{
				return data;
			}
			else
			{
				return 0;
			}
		};

		int8_t readInt8()
		{
			int8_t data;
			size_t readRes = read(&data, sizeof(int8_t));

			if (sizeof(int8_t) == readRes)
			{
				return data;
			}
			else
			{
				return 0;
			}
		};

		void writeUInt8(uint8_t number)
		{
			write(&number, sizeof(uint8_t));
		};

		void writeInt8(int8_t number)
		{
			write(&number, sizeof(int8_t));
		};

		/* 16-bit integer */

		uint16_t readUInt16()
		{
			uint16_t data;
			size_t readRes = read(&data, sizeof(uint16_t));

			if (sizeof(uint16_t) == readRes)
			{
				return data;
			}
			else
			{
				return 0;
			}
		};

		uint16_t readUInt16BE()
		{
			uint16_t data = readUInt16();
			data = SwapEndian<uint16_t>(data);
			return data;
		};

		int16_t readInt16()
		{
			int16_t data;
			size_t readRes = read(&data, sizeof(int16_t));

			if (sizeof(int16_t) == readRes)
			{
				return data;
			}
			else
			{
				return 0;
			}
		};

		int16_t readInt16BE()
		{
			int16_t data = readInt16();
			data = SwapEndian<int16_t>(data);
			return data;
		};

		void writeUInt16(uint16_t number)
		{
			write(&number, sizeof(uint16_t));
		};

		void writeUInt16BE(uint16_t number)
		{
			writeUInt16(SwapEndian<uint16_t>(number));
		};

		void writeInt16(int16_t number)
		{
			write(&number, sizeof(int16_t));
		};

		void writeInt16BE(int16_t number)
		{
			writeInt16(SwapEndian<int16_t>(number));
		};

		/* 32-bit integer */

		uint32_t readUInt32()
		{
			uint32_t data;
			size_t readRes = read(&data, sizeof(uint32_t));

			if (sizeof(uint32_t) == readRes)
			{
				return data;
			}
			else
			{
				return 0;
			}
		};

		uint32_t readUInt32BE()
		{
			uint32_t data = readUInt32();
			data = SwapEndian<uint32_t>(data);
			return data;
		};

		int32_t readInt32()
		{
			int32_t data;
			size_t readRes = read(&data, sizeof(int32_t));

			if (sizeof(int32_t) == readRes)
			{
				return data;
			}
			else
			{
				return 0;
			}
		};

		int32_t readInt32BE()
		{
			uint32_t data = readInt32();
			data = SwapEndian<int32_t>(data);
			return data;
		};

		void writeUInt32(uint32_t number)
		{
			write(&number, sizeof(uint32_t));
		};

		void writeUInt32BE(uint32_t number)
		{
			writeUInt32(SwapEndian<uint32_t>(number));
		};

		void writeInt32(int32_t number)
		{
			write(&number, sizeof(int32_t));
		};

		void writeInt32BE(int32_t number)
		{
			writeInt32(SwapEndian<int32_t>(number));
		};
	};
}