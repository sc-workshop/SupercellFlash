#pragma once

#include "exception/GeneralRuntimeException.h"

namespace sc {
	struct NegativeTagLengthException : public GeneralRuntimeException {
	public:
		NegativeTagLengthException(uint16_t tag, size_t position) {
			std::stringstream message;
			message
				<< "Tag " << tag
				<< " at position " << std::to_string(position)
				<< "not initialized correctly and has a negative length.";

			m_message = message.str();
		}
	};
}
