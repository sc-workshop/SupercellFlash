#pragma once

#include <string>
#include <sstream>

namespace sc {
	struct FileExistException : public std::exception {
		std::string file;
		FileExistException(std::string file) : file(file) {}
		~FileExistException() { }

		const char* what() {
			std::stringstream s;
			s << "File: \"" << file << "\" is missing and cannot be opened." << std::endl;

			return s.str().c_str();
		}
	};
}
