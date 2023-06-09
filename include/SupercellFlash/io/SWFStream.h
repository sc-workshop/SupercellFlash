#pragma once

#include <SupercellBytestream.h>
#include <SupercellCompression.h>

#include <stdexcept>
#include <cstdarg>

namespace sc {
	class SupercellSWF;

	class SWFStream {
		BufferStream m_stream = BufferStream(&buffer);

	public:
		std::vector<uint8_t> buffer;

		SWFStream() {}
		~SWFStream() {}

		void open(const fs::path& filepath) {
			clear();

			fs::path output;
			Decompressor::decompress(filepath, output);
			ReadFileStream file(output);
			buffer = std::vector<uint8_t>(file.size());
			file.read(buffer.data(), file.size());
			file.close();
		}

		void save(const fs::path& filepath, const CompressionSignature& signature) {
			BufferStream input(&buffer);
			WriteFileStream output(filepath);

			switch (signature)
			{
			case sc::CompressionSignature::LZMA:
			case sc::CompressionSignature::LZHAM:
			case sc::CompressionSignature::ZSTD:
				Compressor::compress(input, output, signature);
				break;
			default:
				output.write(buffer.data(), buffer.size());
				break;
			}
			
			clear();
			input.close();
			output.close();
		}

		void clear() {
			buffer.resize(0);
			m_stream.seek(0);
		}

		uint8_t* data() {
			return buffer.data();
		}

		void seek(uint32_t pos) {
			m_stream.seek(pos);
		}

		uint32_t tell() {
			return m_stream.tell();
		}

		void skip(uint32_t size) {
			m_stream.seek(m_stream.tell() + size);
		}

		/* Read */

		void read(void* data, size_t size) {
			m_stream.read(data, size);
		}

		int8_t readByte() { return m_stream.readInt8(); }
		uint8_t readUnsignedByte() { return m_stream.readUInt8(); }

		int16_t readShort() { return m_stream.readInt16(); }
		uint16_t readUnsignedShort() { return m_stream.readUInt16(); }

		int32_t readInt() { return m_stream.readInt32(); }

		bool readBool() { return (readUnsignedByte() > 0); }

		std::string readAscii()
		{
			uint8_t length = readUnsignedByte();
			if (length == 0xFF)
				return "";

			char* str = new char[length]();
			m_stream.read(str, length);

			return std::string(str, length);
		}

		float readTwip() { return (float)readInt() * 0.05f; }

		/* Write */

		void write(void* data, size_t size) {
			m_stream.write(data, size);
		}

		void writeByte(int8_t integer) {
			m_stream.writeInt8(integer);
		}
		void writeUnsignedByte(uint8_t integer) {
			m_stream.writeUInt8(integer);
		}

		void writeShort(int16_t integer) {
			m_stream.writeInt16(integer);
		}
		void writeUnsignedShort(uint16_t integer) {
			m_stream.writeUInt16(integer);
		}

		void writeUnsignedInt(uint32_t integer) {
			m_stream.writeUInt32(integer);
		}

		uint32_t readUnsignedInt() {
			return m_stream.readUInt32();
		}

		void writeInt(int32_t integer) {
			m_stream.writeInt32(integer);
		}

		void writeBool(bool status) {
			m_stream.writeUInt8(status ? 1 : 0);
		}

		void writeAscii(std::string ascii) {
			uint8_t size = ascii.size() == 0 ? 0xFF : static_cast<uint8_t>(ascii.size());

			writeUnsignedByte(size);
			if (size > 0 && size < 255) {
				m_stream.write(ascii.data(), size);
			}
		}

		void writeTwip(float twip) {
			writeInt((int)(twip / 0.05f));
		}

		void writeTag(uint8_t tag) {
			writeUnsignedByte(tag);
			writeInt(0);
		}

		uint32_t initTag() {
			uint32_t res = tell();
			writeUnsignedByte(0xFF);
			writeInt(-1);

			return res;
		}

		void finalizeTag(uint8_t tag, uint32_t position) {
			int32_t tagSize = static_cast<int32_t>(tell() - position - (sizeof(tag) + sizeof(position)));

			memcpy(data() + position, &tag, sizeof(tag));
			memcpy(data() + (position + sizeof(tag)), &tagSize, sizeof(tagSize));
		}
	};
}