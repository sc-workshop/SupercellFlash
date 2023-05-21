#pragma once

#include <string>
#include <sstream>

using namespace std;

namespace sc {
	struct NegativeTagLengthException : public exception {
		string message;

		NegativeTagLengthException(uint16_t tag) {
			stringstream s;
			s << "Tag " << tag << " not initialized correctly and has a negative length." << endl;

			message = s.str();
		}

		const char* what() {
			return message.c_str();
		}
	};
}
