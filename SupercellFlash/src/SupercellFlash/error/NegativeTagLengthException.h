#pragma once

#include "SupercellFlash/error/Exception.h"

namespace sc
{
	class NegativeTagLengthException : public Exception
	{
	public:
		NegativeTagLengthException(const std::string& message) : Exception(message) { }
		virtual ~NegativeTagLengthException() { }
	};
}
