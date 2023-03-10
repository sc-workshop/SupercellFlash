#pragma once

#include <Alloc.h>
#include <LzmaDec.h>
#include <LzmaEnc.h>

#include "SupercellFlash/compression/CompressionError.h"
#include "SupercellFlash/io/ByteStream.h"

namespace sc
{
	class LZMA
	{
	public:
		static CompressionError decompress(ByteStream& inStream, ByteStream& outStream);
		static CompressionError compress(ByteStream& inStream, ByteStream& outStream);

	private:
		static CompressionError decompressStream(CLzmaDec* state, SizeT unpackSize, ByteStream& inStream, ByteStream& outStream);
	};
}