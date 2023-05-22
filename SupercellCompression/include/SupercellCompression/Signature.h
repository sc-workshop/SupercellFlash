#pragma once

namespace sc
{
	enum class CompressionSignature : uint32_t
	{
#ifdef _DEBUG
		NONE = 0,
#endif // DEBUG

		LZMA = 1,
		LZHAM,
		ZSTD,
	};

	inline CompressionSignature getSignature(uint32_t magic)
	{
		switch (magic)
		{
		case 0x5A4C4353:
			return CompressionSignature::LZHAM;

		case 0xFD2FB528:
			return CompressionSignature::ZSTD;

		default:
			return CompressionSignature::LZMA;
		}
	}
}
