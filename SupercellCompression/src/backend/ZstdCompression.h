#pragma once

#include "SupercellBytestream/base/BytestreamBase.h"

namespace sc {
	class ZSTD {
	public:
		static void decompress(BytestreamBase& inStream, BytestreamBase& outStream);
		static void compress(BytestreamBase& inStream, BytestreamBase& outStream);
	};
}
