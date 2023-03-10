#pragma once

#include "SupercellFlash/error/Exception.h"

namespace sc
{
	class DecompressionFailedException : public Exception
	{
	public:
		DecompressionFailedException(const std::string& message) : Exception(message) { }
		virtual ~DecompressionFailedException() { }
	};
}
