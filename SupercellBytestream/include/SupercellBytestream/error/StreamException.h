#pragma once

#include <string>
#include <sstream>

using namespace std;

enum class StreamError {
	UNKNOWN = -1,
	EXIST_ERROR = 1,
	CLOSED_ERROR,
	WRITE_ERROR,
	READ_ERROR,
};

struct StreamException : public exception {
	string message;
	StreamException(StreamError code, string errMessage) {
		stringstream s;
		s << errMessage << endl;
		s << "Exception code: " << (uint8_t)code << endl;
		message = s.str();
	};

	const char* what() {
		return message.c_str();
	}
};