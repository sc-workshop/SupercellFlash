#include "SupercellFlash/compression/backend/LzmaCompression.h"


#include "SupercellFlash/io/ByteStream.h"

static const size_t LZMA_DECOMPRESS_BUF_SIZE = 1 << 16;
static const size_t LZMA_COMPRESS_BUF_SIZE = 1 << 24;

static const size_t SMALL_FILE_SIZE = 1 << 28;
static const uint32_t LZMA_COMPRESS_DICT_SIZE = 262144;

struct CSeqInStreamWrap
{
	ISeqInStream vt;
	sc::ByteStream* inStream;
};

struct CSeqOutStreamWrap
{
	ISeqOutStream vt;
	sc::ByteStream* outStream;
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
	CompressionError LZMA::decompressStream(CLzmaDec* state, SizeT unpackSize, ByteStream& inStream, ByteStream& outStream)
	{
		int hasEndMarker = (unpackSize != (UInt32)(Int32)-1);
		uint8_t inBuf[LZMA_DECOMPRESS_BUF_SIZE];
		uint8_t outBuf[LZMA_DECOMPRESS_BUF_SIZE];
		size_t inPos = 0, inSize = 0, outPos = 0;

		LzmaDec_Init(state);

		while (true)
		{
			if (inPos == inSize)
			{
				inSize = LZMA_DECOMPRESS_BUF_SIZE;
				inStream.read(&inBuf, inSize);
				inPos = 0;
			}
			{
				CompressionError res;
				size_t inProcessed = inSize - inPos;
				size_t outProcessed = LZMA_DECOMPRESS_BUF_SIZE - outPos;
				ELzmaFinishMode finishMode = LZMA_FINISH_ANY;
				ELzmaStatus status;
				if (hasEndMarker && outProcessed > unpackSize)
				{
					outProcessed = (size_t)unpackSize;
					finishMode = LZMA_FINISH_END;
				}

				res = LzmaDec_DecodeToBuf(state, outBuf + outPos, &outProcessed,
					inBuf + inPos, &inProcessed, finishMode, &status) == 0 ? CompressionError::Ok : CompressionError::DataError;
				inPos += inProcessed;
				outPos += outProcessed;
				unpackSize -= outProcessed;

				if (outStream.write(&outBuf, outPos) != outPos)
					return CompressionError::DataError;

				outPos = 0;

				if (res != CompressionError::Ok || (hasEndMarker && unpackSize == 0))
					return res;

				if (inProcessed == 0 && outProcessed == 0)
				{
					if (hasEndMarker || status != LZMA_STATUS_FINISHED_WITH_MARK)
						return CompressionError::DataError;

					return res;
				}
			}
		}
	}

	CompressionError LZMA::decompress(ByteStream& inStream, ByteStream& outStream)
	{
		CLzmaDec state;
		Byte header[LZMA_PROPS_SIZE];
		inStream.read(header, LZMA_PROPS_SIZE);

		unsigned int unpackSize = 0;
		inStream.read(&unpackSize, sizeof(unpackSize));

		LzmaDec_Construct(&state);
		LzmaDec_Allocate(&state, header, LZMA_PROPS_SIZE, &g_Alloc);

		CompressionError res = decompressStream(&state, unpackSize, inStream, outStream);

		LzmaDec_Free(&state, &g_Alloc);

		return res;
	}

	CompressionError LZMA::compress(ByteStream& inStream, ByteStream& outStream)
	{
		CLzmaEncHandle enc;
		SRes res;
		CLzmaEncProps props;
		inStream.seek(0);

		enc = LzmaEnc_Create(&g_Alloc);
		if (enc == 0)
			return CompressionError::AllocError;

		LzmaEncProps_Init(&props);
		props.level = 6;
		props.pb = 2;
		props.lc = 3;
		props.lp = 0;
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

			auto outStreamSize = static_cast<uint32_t>(inStream.size());

			outStream.writeInt(outStreamSize);

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

		return CompressionError::Ok;
	}
}
