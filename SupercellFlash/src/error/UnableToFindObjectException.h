#pragma once 

#include <string>
#include <sstream>

namespace sc {
	struct UnableToFindObjectException : public std::exception {
		std::string name;
		UnableToFindObjectException(std::string name) : name(name) {}
		~UnableToFindObjectException() { }

		const char* what() {
			std::stringstream s;
			s << "Failed to find object with name: \"" << name << "\"" << std::endl;

			return s.str().c_str();
		}
	};
}

