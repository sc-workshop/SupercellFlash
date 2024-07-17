#pragma once
#include <optional>

#include "SupercellFlash/types/SWFString.hpp"
#include "exception/GeneralRuntimeException.h"

namespace sc {
	class ObjectNotFoundException : public GeneralRuntimeException {
	public:
		ObjectNotFoundException(uint16_t id) : GeneralRuntimeException("DisplayObjectNotFoundExceptinon")
		{
			std::stringstream message;
			message << "Failed to get Display Object by ID: " << id;
			m_message = message.str();
		}

		ObjectNotFoundException(SWFString name) : GeneralRuntimeException("ExportNameNotFoundException")
		{
			std::stringstream message;
			message << "Failed to get Export Name ID: " << name.string();
			m_message = message.str();
		}
	};
}
