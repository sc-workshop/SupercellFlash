#pragma once

#include "SupercellFlash/compression/CompressionError.h"
#include "SupercellFlash/io/ByteStream.h"

namespace sc {
	class ZSTD
	{
	public:
		static CompressionError decompress(ByteStream& inStream, ByteStream& outStream);
		static CompressionError compress(ByteStream& inStream, ByteStream& outStream);
	};
}