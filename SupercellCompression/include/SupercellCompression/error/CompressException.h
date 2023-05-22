#pragma once

#include <string>

namespace sc {
	struct CompressException : public std::exception {
		std::string message;
		CompressException(std::string msg) : message(msg) {}

		const char* what() const override {
			return message.c_str();
		}
	};
}