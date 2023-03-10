#pragma once

#include "SupercellFlash/error/Exception.h"

namespace sc
{
	class UnknownFileVersionException : public Exception
	{
	public:
		UnknownFileVersionException(const std::string& message) : Exception(message) { }
		virtual ~UnknownFileVersionException() { }
	};
}
