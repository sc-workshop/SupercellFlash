#pragma once

#include <string>

namespace sc {
	struct DecompressException : public std::exception {
		std::string message;
		DecompressException(std::string msg) : message(msg) {}

		const char* what() const override {
			return message.c_str();
		}
	};
}