#pragma once

#include "SupercellFlash/error/Exception.h"

namespace sc
{
	class UnknownFileMagicException : public Exception
	{
	public:
		UnknownFileMagicException(const std::string& message) : Exception(message) { }
		virtual ~UnknownFileMagicException() { }
	};
}
