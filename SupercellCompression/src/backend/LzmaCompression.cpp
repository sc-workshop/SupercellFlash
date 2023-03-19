#include "LzmaCompression.h"

#include "SupercellCompression/error/DecompressException.h"
#include "SupercellCompression/error/CompressException.h"

static const size_t LZMA_DECOMPRESS_BUF_SIZE = 1 << 16;
static const size_t LZMA_COMPRESS_BUF_SIZE = 1 << 24;

static const size_t SMALL_FILE_SIZE = 1 << 28;
static const uint32_t LZMA_COMPRESS_DICT_SIZE = 262144;

struct CSeqInStreamWrap
{
	ISeqInStream vt;
	sc::BytestreamBase* inStream;
};

struct CSeqOutStreamWrap
{
	ISeqOutStream vt;
	sc::BytestreamBase* outStream;
};

static SRes LzmaStreamRead(const ISeqInStream* p, void* data, size_t* size)
{
	CSeqInStreamWrap* wrap = CONTAINER_FROM_VTBL(p, CSeqInStreamWrap, vt);
	size_t bufferReadSize = (*size < LZMA_COMPRESS_BUF_SIZE) ? *size : LZMA_COMPRESS_BUF_SIZE;
	size_t readSize = wrap->inStream->read(data, bufferReadSize);

	*size = readSize;
	return SZ_OK;
}

static size_t LzmaStreamWrite(const ISeqOutStream* p, const void* buf, size_t size)
{
	auto* wrap = CONTAINER_FROM_VTBL(p, CSeqOutStreamWrap, vt);
	return wrap->outStream->write((void*)buf, size);
}

namespace sc
{
	void LZMA::decompressStream(CLzmaDec* state, SizeT unpackedSize, BytestreamBase& inStream, BytestreamBase& outStream)
	{
		int hasBound = (unpackedSize != (UInt32)(Int32)-1);

		uint8_t inBuffer[LZMA_DECOMPRESS_BUF_SIZE];
		uint8_t outBuffer[LZMA_DECOMPRESS_BUF_SIZE];

		LzmaDec_Init(state);

		size_t inPos = 0, inSize = 0, outPos = 0;
		while (true)
		{
			if (inPos == inSize)
			{
				inSize = LZMA_DECOMPRESS_BUF_SIZE;
				inStream.read(&inBuffer, inSize);
				inPos = 0;
			}
			{
				SRes res;
				size_t inProcessed = inSize - inPos;
				size_t outProcessed = LZMA_DECOMPRESS_BUF_SIZE - outPos;
				ELzmaFinishMode finishMode = LZMA_FINISH_ANY;
				ELzmaStatus status;
				if (hasBound && outProcessed > unpackedSize)
				{
					outProcessed = (size_t)unpackedSize;
					finishMode = LZMA_FINISH_END;
				}

				res = LzmaDec_DecodeToBuf(state, outBuffer + outPos, &outProcessed,
					inBuffer + inPos, &inProcessed, finishMode, &status);
				inPos += inProcessed;
				outPos += outProcessed;
				unpackedSize -= outProcessed;

				if (outStream.write(&outBuffer, outPos) != outPos || res != SZ_OK)
					throw DecompressException("Corrupted data in LZMA decompress");

				outPos = 0;

				if (hasBound && unpackedSize == 0)
					return;

				if (inProcessed == 0 && outProcessed == 0)
				{
					if (hasBound || status != LZMA_STATUS_FINISHED_WITH_MARK)
						throw DecompressException("Decompress LZMA stream finished without END MARKER");

					return;
				}
			}
		}
	}

	void LZMA::decompress(BytestreamBase& inStream, BytestreamBase& outStream)
	{
		CLzmaDec state;
		Byte header[LZMA_PROPS_SIZE];
		inStream.read(header, LZMA_PROPS_SIZE);

		unsigned int unpackSize = 0;
		inStream.read(&unpackSize, sizeof(unpackSize));

		LzmaDec_Construct(&state);
		LzmaDec_Allocate(&state, header, LZMA_PROPS_SIZE, &g_Alloc);

		decompressStream(&state, unpackSize, inStream, outStream);

		LzmaDec_Free(&state, &g_Alloc);
	}

	void LZMA::compress(BytestreamBase& inStream, BytestreamBase& outStream, int16_t theards)
	{
		CLzmaEncHandle enc;
		SRes res;
		CLzmaEncProps props;
		enc = LzmaEnc_Create(&g_Alloc);
		if (enc == 0)
			throw CompressException("Failed to initialize LZMA compress stream");

		LzmaEncProps_Init(&props);
		props.level = 6;
		props.pb = 2;
		props.lc = 3;
		props.lp = 0;
#ifdef SC_MULTITHEARD
		props.numThreads = theards > 0 ? theards : 1;
#endif // !SC_MULTITHEARD

		props.dictSize = LZMA_COMPRESS_DICT_SIZE;

		if (inStream.size() > SMALL_FILE_SIZE)
			props.lc = 4;

		LzmaEncProps_Normalize(&props);
		res = LzmaEnc_SetProps(enc, &props);

		if (res == SZ_OK)
		{
			uint8_t header[LZMA_PROPS_SIZE];
			size_t headerSize = LZMA_PROPS_SIZE;
			res = LzmaEnc_WriteProperties(enc, header, &headerSize);

			outStream.write(&header, headerSize);

			uint32_t outStreamSize = static_cast<uint32_t>(inStream.size());

			outStream.write(&outStreamSize, sizeof(outStreamSize));

			// Read stream wrap
			CSeqInStreamWrap inWrap = {};
			inWrap.vt.Read = LzmaStreamRead;
			inWrap.inStream = &inStream;

			// Write stream wrap
			CSeqOutStreamWrap outWrap = {};
			outWrap.vt.Write = LzmaStreamWrite;
			outWrap.outStream = &outStream;
			LzmaEnc_Encode(enc, &outWrap.vt, &inWrap.vt, nullptr, &g_Alloc, &g_Alloc);
		}

		LzmaEnc_Destroy(enc, &g_Alloc, &g_Alloc);
	}
}