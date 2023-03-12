#include "LzhamCompression.h"

#include "error/CompressException.h"
#include "error/DecompressException.h"

#include <lzham_static_lib.h>

uint32_t LZHAM_MAGIC = 0x5A4C4353;
uint8_t DICT_SIZE = 18;

constexpr size_t LZHAM_COMP_INPUT_BUFFER_SIZE = 65536 * 4;
constexpr size_t LZHAM_COMP_OUTPUT_BUFFER_SIZE = 65536*4;

#define LZHAM_DECOMP_INPUT_BUFFER_SIZE 65536*4
#define LZHAM_DECOMP_OUTPUT_BUFFER_SIZE 65536*4

#define my_max(a,b) (((a) > (b)) ? (a) : (b))
#define my_min(a,b) (((a) < (b)) ? (a) : (b))

namespace sc
{
	void LZHAM::compress(BytestreamBase& inStream, BytestreamBase& outStream)
	{
		inStream.set(0);
		uint32_t fileSize = inStream.size();

		const uint32_t inBufferSize = LZHAM_COMP_INPUT_BUFFER_SIZE;
		const uint32_t outBufferSize = LZHAM_COMP_OUTPUT_BUFFER_SIZE;

		uint8_t* inBuffer = static_cast<uint8_t*>(malloc(inBufferSize));
		uint8_t* outBuffer = static_cast<uint8_t*>(malloc(outBufferSize));
		if ((!inBuffer) || (!outBuffer))
		{
			throw CompressException("Failed to allocate memory for LZHAM compress stream");
		}

		uint64_t srcLeft = static_cast<uint64_t>(fileSize);

		uint32_t inBufferPos = 0;
		uint32_t inBufferOffset = 0;

		lzham_compress_params params;
		memset(&params, 0, sizeof(params));
		params.m_struct_size = sizeof(lzham_compress_params);
		params.m_dict_size_log2 = DICT_SIZE;
		// params.m_max_helper_threads = 2;

		lzham_compress_state_ptr lzhamState = lzham_compress_init(&params);

		if (!lzhamState)
		{
			free(inBuffer);
			free(outBuffer);
			throw CompressException("Failed to initialize LZHAM stream");
		}

		lzham_compress_status_t status = LZHAM_COMP_STATUS_FAILED;

		outStream.write(&LZHAM_MAGIC, sizeof(LZHAM_MAGIC));
		outStream.write(&DICT_SIZE, sizeof(DICT_SIZE));
		outStream.write(&fileSize, sizeof(fileSize));

		while(true)
		{
			if (inBufferOffset == inBufferPos)
			{
				inBufferPos = static_cast<uint32_t>(my_min(inBufferSize, srcLeft));
				if (inStream.read(inBuffer, inBufferPos) != inBufferPos)
				{
					free(inBuffer);
					free(outBuffer);
					lzham_compress_deinit(lzhamState);
					throw CompressException("Corrupted LZHAM data in compress stream");
				}

				srcLeft -= inBufferPos;

				inBufferOffset = 0;
			}

			uint8_t* inBytes = &inBuffer[inBufferOffset];
			size_t inBytesCount = inBufferPos - inBufferOffset;
			uint8_t* outBytes = outBuffer;
			size_t outBytesCount = outBufferSize;

			status = lzham_compress(lzhamState, inBytes, &inBytesCount, outBytes, &outBytesCount, srcLeft == 0);

			if (inBytesCount)
			{
				inBufferOffset += (uint8_t)inBytesCount;
			}

			if (outBytesCount)
			{
				if (outStream.write(outBuffer, outBytesCount) != outBytesCount)
				{
					free(inBuffer);
					free(outBuffer);
					lzham_compress_deinit(lzhamState);
					throw CompressException("Corrupted data in LZHAM compress stream");
				}
			}

			if (status >= LZHAM_COMP_STATUS_FIRST_SUCCESS_OR_FAILURE_CODE)
				break;
		}

		free(inBuffer);
		inBuffer = NULL;
		free(outBuffer);
		outBuffer = NULL;
	}

	void LZHAM::decompress(BytestreamBase& inStream, BytestreamBase& outStream)
	{
		uint32_t magic;
		inStream.read(&magic, sizeof(magic));

		if (magic != 0x5A4C4353)
			throw DecompressException("Wrong header magic in LZHAM data");

		uint8_t dictSize;
		uint32_t fileSize;

		inStream.read(&dictSize, sizeof(dictSize));
		inStream.read(&fileSize, sizeof(fileSize));

		if ((dictSize < LZHAM_MIN_DICT_SIZE_LOG2) || (dictSize > LZHAM_MAX_DICT_SIZE_LOG2_X64))
		{
			throw DecompressException("Wrong dict size in LZHAM decompress stream");
		}

		const uint32_t inputBufferSize = LZHAM_DECOMP_INPUT_BUFFER_SIZE;
		uint8_t* inputBuffer = static_cast<uint8_t*>(malloc(inputBufferSize));

		uint32_t outputBufferSize = LZHAM_DECOMP_OUTPUT_BUFFER_SIZE;
		uint8_t* outputBuffer = static_cast<uint8_t*>(malloc(outputBufferSize));
		if (!outputBuffer)
		{
			free(inputBuffer);
			throw DecompressException("Failed to allocate memory for LZHAM decompress stream");
		}

		uint32_t inputLeft = inStream.size() - inStream.tell();
		size_t outputLeft = static_cast<size_t>(fileSize);

		uint32_t decompressBufferSize = 0;
		uint32_t decompressBufferOffset = 0;

		lzham_decompress_params params;
		memset(&params, 0, sizeof(params));
		params.m_struct_size = sizeof(lzham_decompress_params);
		params.m_dict_size_log2 = dictSize;
		params.m_table_update_rate = 8;

		lzham_decompress_state_ptr decompressState = lzham_decompress_init(&params);
		if (!decompressState)
		{
			free(inputBuffer);
			free(outputBuffer);
			throw DecompressException("Failed to initialize LZHAM decompress stream");
		}

		lzham_decompress_status_t lzham_status;
		while(true)
		{
			if (decompressBufferOffset == decompressBufferSize)
			{
				decompressBufferSize = static_cast<uint32_t>(inputBufferSize < inputLeft ? inputBufferSize : inputLeft);
				if (inStream.read(inputBuffer, decompressBufferSize) != decompressBufferSize)
				{
					free(inputBuffer);
					free(outputBuffer);
					lzham_decompress_deinit(decompressState);
					throw DecompressException("Corrupted data in LZHAM decompress stream");
				}

				inputLeft -= decompressBufferSize;

				decompressBufferOffset = 0;
			}

			uint8_t* inBytes = &inputBuffer[decompressBufferOffset];
			size_t inBytesCount = decompressBufferSize - decompressBufferOffset;
			uint8_t* outBytes = outputBuffer;
			size_t outBytesCount = outputBufferSize;

			lzham_status = lzham_decompress(decompressState, inBytes, &inBytesCount, outBytes, &outBytesCount, inputLeft == 0);

			if (inBytesCount)
			{
				decompressBufferOffset += (uint32_t)inBytesCount;
			}

			if (outBytesCount)
			{
				if (outStream.write(outputBuffer, static_cast<uint32_t>(outBytesCount)) != outBytesCount)
				{
					free(inputBuffer);
					free(outputBuffer);
					lzham_decompress_deinit(decompressState);
					throw DecompressException("Failed to write data to output LZHAM decompres stream");
				}

				if (outBytesCount > outputLeft)
				{
					free(inputBuffer);
					free(outputBuffer);
					lzham_decompress_deinit(decompressState);
					throw DecompressException("Too many bytes left in LZHAM decompress stream");
				}
				outputLeft -= outBytesCount;
			}

			if (lzham_status >= LZHAM_DECOMP_STATUS_FIRST_SUCCESS_OR_FAILURE_CODE)
				break;
		}

		free(inputBuffer);
		inputBuffer = NULL;

		free(outputBuffer);
		outputBuffer = NULL;

		lzham_decompress_deinit(decompressState);
		decompressState = NULL;
	}
}