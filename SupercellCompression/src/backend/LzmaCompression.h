#pragma once

#include <Alloc.h>
#include <LzmaDec.h>
#include <LzmaEnc.h>

#include <SupercellBytestream/base/BytestreamBase.h>

namespace sc
{
	class LZMA
	{
	public:
		static void decompress(BytestreamBase& inStream, BytestreamBase& outStream);
		static void compress(BytestreamBase& inStream, BytestreamBase& outStream, int16_t theards);

	private:
		static void decompressStream(CLzmaDec* state, SizeT unpackSize, BytestreamBase& inStream, BytestreamBase& outStream);
	};
}