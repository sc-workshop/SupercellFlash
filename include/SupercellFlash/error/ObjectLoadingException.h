#pragma once

#include <string>
#include <sstream>

using namespace std;

namespace sc {
	struct ObjectLoadingException : public exception {
		string message;
		ObjectLoadingException(string message) : message(message) {}
		~ObjectLoadingException() { }

		const char* what() const noexcept override {
			return message.c_str();
		}
	};
}
