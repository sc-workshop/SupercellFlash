#pragma once

#include <memory/alloc.h>
#include <string>

#include "io/stream.h"

namespace sc
{
	class SWFString
	{
	public:
		SWFString() {};

		SWFString(const char* data)
		{
			size_t string_size = strlen(data);
			m_length = string_size >= 0xFE ? 0xFE : static_cast<uint8_t>(string_size);
			m_data = memalloc<char>(m_length + 1);
			memcopy(data, m_data, m_length);
			*(m_data + m_length) = '\0';
		}

		SWFString(std::string& string)
		{
			m_length = string.length() >= 0xFE ? 0xFE : static_cast<uint8_t>(string.length());

			m_data = memalloc<char>(m_length);
			memcopy(string.data(), m_data, m_length);
		}

		SWFString(const SWFString& string)
		{
			m_length = string.length();
			m_data = memalloc<char>(m_length);
			memcopy(string.data(), m_data, m_length);
		}

		~SWFString()
		{
			clear();
		}

	public:
		bool empty() const
		{
			return m_data == nullptr || m_length == 0;
		}

		uint8_t length() const
		{
			return m_length;
		}

		char* data() const
		{
			return (char*)(m_data);
		}

		std::string string() const
		{
			return std::string((const char*)m_data, (const char*)m_data + m_length);
		}

		void clear()
		{
			if (m_data)
			{
				free(m_data);
				m_data = nullptr;
			}
			m_length = 0;
		}

		int compare(const char* string, size_t len = 0) const
		{
			uint8_t string_size = static_cast<uint8_t>(len);
			if (string_size == 0)
			{
				string_size = static_cast<uint8_t>(strlen(string));
			}

			if (m_length != string_size)
			{
				if (m_length > string_size)
				{
					return 1;
				}
				else
				{
					return -1;
				}
			}

			for (uint16_t i = 0; m_length > i; i++)
			{
				const char* lsymbol = string + i;
				const char* rsymbol = m_data + i;

				if (*lsymbol != *rsymbol)
				{
					return -1;
				}
			}

			return 0;
		}

		int compare(const std::string& string) const
		{
			return compare(string.c_str(), string.size());
		}

		void resize(uint8_t new_length, char fill = '\0')
		{
			if (m_length == new_length)
			{
				return;
			}

			if (new_length == 0xFF)
			{
				new_length = 0;
			}

			if (new_length == 0)
			{
				free(m_data);
				m_data = nullptr;
				m_length = 0;
			}

			//if (m_length > new_length)
			//{
			//	m_length = new_length;
			//
			//	*(m_data + new_length) = '\0';
			//}
			//else
			//{
			//	char* new_data = memalloc<char>(new_length);
			//
			//	for (uint8_t i = 0; new_length > i; i++)
			//	{
			//		*(new_data + i) = i < m_length ? *(m_data + i) : fill;
			//	}
			//
			//	free(m_data);
			//	m_data = new_data;
			//}

			char* new_data = memalloc<char>(new_length + 1);

			for (uint8_t i = 0; new_length > i; i++)
			{
				*(new_data + i) = i >= m_length ? fill : *(m_data + i);
			}
			*(new_data + new_length) = '\0';

			clear();
			m_data = new_data;
			m_length = new_length;
		}

	private:
		char* m_data = nullptr;
		uint8_t m_length = 0;
	};
}