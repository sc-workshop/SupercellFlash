#pragma once

#include <string>
#include <sstream>

namespace sc {
	struct NegativeTagLengthException : public std::exception {
		uint16_t tag;
		NegativeTagLengthException(uint16_t tag) : tag(tag) {}
		~NegativeTagLengthException() { }

		const char* what() {
			std::stringstream s;
			s << "Tag " << tag << " not initialized correctly and has a negative length." << std::endl;

			return s.str().c_str();
		}
	};
}
