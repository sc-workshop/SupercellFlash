#include "ZstdCompression.h"

#include "error/DecompressException.h"
#

#include <zstd.h>
#include <thread>

namespace sc {
	void ZSTD::decompress(BytestreamBase& inStream, BytestreamBase& outStream) {
		const size_t inBufferSize = ZSTD_DStreamInSize();
		const size_t outBufferSize = ZSTD_DStreamOutSize();

		void* inBuffer = malloc(inBufferSize);
		void* outBuffer = malloc(outBufferSize);

		if (!inBuffer || !outBuffer) {
			throw DecompressException("Failed to allocate buffer in ZSTD decompress");
		};

		ZSTD_DStream* const dctx = ZSTD_createDStream();

		if (!dctx)
		{
			throw DecompressException("Failed to create ZSTD stream in decompress");
		}

		size_t dRes = ZSTD_initDStream(dctx);

		if (ZSTD_isError(dRes))
		{
			ZSTD_freeDStream(dctx);
			throw DecompressException("Failed to initialize ZSTD stream in decompress");
		}

		uint64_t unpackedSize = ZSTD_getDecompressedSize(inStream.data(), inStream.size());
		if (unpackedSize == 0) {
			unpackedSize = inStream.size() - inStream.tell();
		}

		ZSTD_inBuffer zInBuffer = { 0 };
		ZSTD_outBuffer zOutBuffer = { 0 };
		size_t chunkSize = 0;
		size_t totalBytes = 0;

		while (totalBytes < unpackedSize) {
			chunkSize = inStream.read(inBuffer, inBufferSize);
			if (!chunkSize) {
				break;
			}

			zInBuffer.src = inBuffer;
			zInBuffer.size = chunkSize;
			zInBuffer.pos = 0;

			while (zInBuffer.pos < zInBuffer.size && totalBytes < unpackedSize) {
				zOutBuffer.dst = outBuffer;
				zOutBuffer.size = outBufferSize;
				zOutBuffer.pos = 0;
				dRes = ZSTD_decompressStream(dctx, &zOutBuffer, &zInBuffer);
				if (ZSTD_isError(dRes)) {
					throw DecompressException("Corrupted data in ZSTD decompress stream");
				}
				outStream.write(outBuffer, zOutBuffer.pos);
				totalBytes += zOutBuffer.pos;
			}
		}

		ZSTD_freeDStream(dctx);
		free(inBuffer);
		free(outBuffer);
	}

	void ZSTD::compress(BytestreamBase& inStream, BytestreamBase& outStream)
	{
		size_t const buffInSize = ZSTD_CStreamInSize();
		size_t const buffOutSize = ZSTD_CStreamOutSize();

		void* buffIn = malloc(buffInSize);
		void* buffOut = malloc(buffOutSize);

		if (!buffIn || !buffOut) {
			throw DecompressException("Failed to allocate buffer in ZSTD decompress");
		}

		ZSTD_CCtx* const cctx = ZSTD_createCCtx();
		if (cctx == NULL) {
			throw DecompressException("Failed to create ZSTD compress context");
		}

		ZSTD_CCtx_setParameter(cctx, ZSTD_c_compressionLevel, 16);
		ZSTD_CCtx_setParameter(cctx, ZSTD_c_checksumFlag, 0);

		ZSTD_CCtx_setParameter(cctx, ZSTD_c_contentSizeFlag, 1);
		ZSTD_CCtx_setPledgedSrcSize(cctx, inStream.size());

		size_t const remainBytes = buffInSize;
		while(true) {
			size_t byteCount = inStream.read(buffIn, remainBytes);

			int const lastChunk = (byteCount < remainBytes);
			ZSTD_EndDirective const mode = lastChunk ? ZSTD_e_end : ZSTD_e_continue;
			ZSTD_inBuffer input = { buffIn, byteCount, 0 };
			int finished = 0;
			while (!finished) {
				ZSTD_outBuffer output = { buffOut, buffOutSize, 0 };
				size_t const remaining = ZSTD_compressStream2(cctx, &output, &input, mode);
				outStream.write(buffOut, output.pos);
				finished = lastChunk ? (remaining == 0) : (input.pos == input.size);
			};
			if (input.pos != input.size) {
				throw DecompressException("Corrupted data in ZSTD compression");
			}

			if (lastChunk) {
				break;
			}
		}

		ZSTD_freeCCtx(cctx);
		free(buffIn);
		free(buffOut);
	}
}