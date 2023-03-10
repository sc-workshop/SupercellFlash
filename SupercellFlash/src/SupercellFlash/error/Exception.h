#pragma once

#include <stdexcept>
#include <string>

namespace sc
{
	class Exception : public std::exception
	{
	public:
		Exception(const std::string& message) : m_message(message) { }
		virtual ~Exception() { }

	public:
		const char* what() const override { return m_message.c_str(); }
		
	private:
		std::string m_message;
	};
}
