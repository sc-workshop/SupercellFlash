#pragma once

#include <core/io/buffer_stream.h>
#include <core/io/file_stream.h>
#include <compression/compression.h>

#include "SWFString.hpp"

namespace sc
{
	namespace flash {
		using namespace sc::compression::flash;
		class SWFStream : public BufferStream
		{
		public:
			SWFStream() {};
			SWFStream(const SWFStream&) {};
			SWFStream& operator=(const SWFStream&) { return *this; };

		public:
#pragma region File IO
		public:
			void open_file(const std::filesystem::path& path)
			{
				clear();

				InputFileStream file(path);
				Decompressor::decompress(file, *this);

				seek(0);
			}

			void save_file(const std::filesystem::path& path, Signature signature)
			{
				OutputFileStream file(path);

				Compressor::Context context;
				context.signature = signature;

				seek(0);
				Compressor::compress(*this, file, context);
				clear();
			}
#pragma endregion

#pragma region Writing Functions
		public:
			void inline write_string(const SWFString& string)
			{
				uint8_t string_size = string.length();
				if (string_size)
				{
					write_unsigned_byte(string_size);
					write(string.data(), string_size);
				}
				else
				{
					write_unsigned_byte(0xFF);
				}
			}

			void inline write_twip(float twip) {
				write_int((int)(twip / 0.05f));
			}

			size_t inline write_tag_header(uint8_t tag)
			{
				write_unsigned_byte(tag);
				write_int(-1);
				return position();
			}

			void inline write_tag_final(size_t tag_start)
			{
				int* tag_length = (int*)((uint8_t*)data() + (tag_start - 4));
				*tag_length = static_cast<int>(position() - tag_start);
			}

			void inline write_tag_flag(uint8_t tag)
			{
				write_unsigned_byte(tag);
				write_int(0);
			}

#pragma endregion

#pragma region Reading Functions
		public:
			void inline read_string(SWFString& string)
			{
				uint8_t string_size = read_unsigned_byte();

				if (string_size != 0xFF)
				{
					string.resize(string_size);
					read(string.data(), string_size);
					return;
				}

				string.resize(0);
			}

			float inline read_twip()
			{
				return (float)read_int() * 0.05f;
			}
#pragma endregion
		};
	}
}