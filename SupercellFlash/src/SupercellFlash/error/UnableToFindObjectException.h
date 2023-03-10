#pragma once

#include "SupercellFlash/error/Exception.h"

namespace sc
{
	class UnableToFindObjectException : public Exception
	{
	public:
		UnableToFindObjectException(const std::string& message) : Exception(message) { }
		virtual ~UnableToFindObjectException() { }
	};
}
