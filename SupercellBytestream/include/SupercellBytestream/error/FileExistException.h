#pragma once

#include <string>
#include <sstream>

using namespace std;

namespace sc {
	struct FileExistException : public exception {
		string message;
		FileExistException(string file) {
			stringstream s;
			s << "File: \"" << file << "\" is missing and cannot be opened." << endl;
			message = s.str();
		}

		const char* what() {
			return message.c_str();
		}
	};
}
