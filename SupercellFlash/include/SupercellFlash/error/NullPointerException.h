#pragma once

#include <typeinfo>
#include <string>
#include <sstream>

using namespace std;

namespace sc {
	template<typename T>
	class NullPointerException : public exception {
		string message;

	public:
		NullPointerException() {
			std::stringstream s;
			s << "Failed to get pointer of \"" << typeid(T).name() << "\"" << endl;
			message = s.str();
		}
	
		const char* what() const override {
			return message.c_str();
		}
	};
}
