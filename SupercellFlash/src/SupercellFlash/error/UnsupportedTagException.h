#pragma once

#include "SupercellFlash/error/Exception.h"

namespace sc
{
	class UnsupportedTagException : public Exception
	{
	public:
		UnsupportedTagException(const std::string& message) : Exception(message) { }
		virtual ~UnsupportedTagException() { }
	};
}
