#pragma once

#include "SupercellFlash/error/Exception.h"

namespace sc
{
	class LoadingFaultException : public Exception
	{
	public:
		LoadingFaultException(const std::string& message) : Exception(message) { }
		virtual ~LoadingFaultException() { }
	};
}
