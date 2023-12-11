#pragma once

#include "exception/GeneralRuntimeException.h"

namespace sc {
	class ObjectLoadingException : public GeneralRuntimeException {
	public:
		ObjectLoadingException(const char* message)
		{
			m_message = message;
		}
	};
}
