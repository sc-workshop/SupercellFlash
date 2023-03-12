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

		ZSTD_DStream* const dStream = ZSTD_createDStream();

		if (!dStream)
		{
			throw DecompressException("Failed to create ZSTD stream in decompress");
		}

		size_t dRes = ZSTD_initDStream(dStream);

		if (ZSTD_isError(dRes))
		{
			ZSTD_freeDStream(dStream);
			throw DecompressException("Failed to initialize ZSTD stream in decompress");
		}

		/*while (const size_t read = inStream.read(buffIn, buffInSize)) {
			ZSTD_inBuffer input = { buffIn, read, 0 };

			while (input.pos < input.size)
			{
				ZSTD_outBuffer output = { buffOut, buffOutSize, 0 };
				dRes = ZSTD_decompressStream(dStream, &output, &input);

				if (ZSTD_isError(dRes))
				{
					ZSTD_freeDStream(dStream);
					throw DecompressException("Broken data in ZSTD decompression");
				}
				outStream.write(buffOut, output.pos);
			}
		}*/

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

			while (zInBuffer.pos < zInBuffer.size) {
				zOutBuffer.dst = outBuffer;
				zOutBuffer.size = outBufferSize;
				zOutBuffer.pos = 0;
				dRes = ZSTD_decompressStream(dStream, &zOutBuffer, &zInBuffer);
				if (ZSTD_isError(dRes)) {
					throw DecompressException("Corrupted data in ZSTD decompress stream");
					break;
				}
				outStream.write(outBuffer, zOutBuffer.pos);
				totalBytes += zOutBuffer.pos;
			}
		}

		ZSTD_freeDStream(dStream);
		free(inBuffer);
		free(outBuffer);
	}

	void ZSTD::compress(BytestreamBase& inStream, BytestreamBase& outStream)
	{
		inStream.set(0);
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

		size_t const toRead = buffInSize;
		for (;;) {
			size_t read = inStream.read(buffIn, toRead);

			int const lastChunk = (read < toRead);
			ZSTD_EndDirective const mode = lastChunk ? ZSTD_e_end : ZSTD_e_continue;
			ZSTD_inBuffer input = { buffIn, read, 0 };
			int finished;
			do {
				ZSTD_outBuffer output = { buffOut, buffOutSize, 0 };
				size_t const remaining = ZSTD_compressStream2(cctx, &output, &input, mode);
				outStream.write(buffOut, output.pos);
				finished = lastChunk ? (remaining == 0) : (input.pos == input.size);
			} while (!finished);
			if (input.pos != input.size) {
				throw DecompressException("Broken data in ZSTD compression");
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