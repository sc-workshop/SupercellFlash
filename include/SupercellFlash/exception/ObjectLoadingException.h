#pragma once

#include "exception/GeneralRuntimeException.h"

namespace sc {
	class ObjectLoadingException : public GeneralRuntimeException {
	public:
		ObjectLoadingException(const char* message) : GeneralRuntimeException("ObjectLoadingException")
		{
			m_message = message;
		}
	};
}
