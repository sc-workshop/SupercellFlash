#pragma once

#include <SupercellBytestream/base/BytestreamBase.h>

namespace sc
{
	class LZHAM
	{
	public:
		static void compress(BytestreamBase& inStream, BytestreamBase& outStream, int16_t theards);
		static void decompress(BytestreamBase& inStream, BytestreamBase& outStream);
	};
}