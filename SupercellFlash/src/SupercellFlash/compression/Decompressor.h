#pragma once

#include <vector>

#include "SupercellFlash/io/ByteStream.h"

namespace sc
{
	class Decompressor
	{
	public:
		static void decompress(ByteStream& inStream, ByteStream& outStream);
	};
}
