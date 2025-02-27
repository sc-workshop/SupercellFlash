#pragma once

#include <string>
#include <optional>

#include "core/memory/memory.h"
#include "core/memory/ref.h"
#include "core/io/stream.h"
#include "core/hashing/hash_stream.h"

namespace sc
{
	namespace flash
	{
		class SWFString
		{
		public:
			SWFString() {};

			SWFString(const char* data, std::optional<size_t> length = std::nullopt)
			{
				size_t string_size = 0;
				if (length.has_value())
				{
					string_size = length.value();
				}
				else
				{
					string_size = strlen(data);
				}

				m_length = string_size >= 0xFE ? 0xFE : static_cast<uint8_t>(string_size);
				if (!m_length) return;

				char* data_ptr = wk::Memory::allocate<char>(m_length + 1);
				m_data = wk::Ref<char>(data_ptr);
				wk::Memory::copy(data, data_ptr, m_length);
				*(data_ptr + m_length) = '\0';
			}

			SWFString(const std::string& string_data)
			{
				std::string string = string_data;
				string.erase(
					std::remove_if(string.begin(), string.end(),
					[](char value) { 
							return !SWFString::IsValid(value);
						}
					),
					string.end()
				);

				m_length = string.length() >= 0xFE ? 0xFE : static_cast<uint8_t>(string.length());
				if (!m_length) return;

				char* data_ptr = wk::Memory::allocate<char>(m_length + 1);
				m_data = wk::Ref<char>(data_ptr);

				wk::Memory::copy(string.c_str(), data_ptr, m_length + 1);
			}
			SWFString& operator=(const SWFString&) = default;

			SWFString(const SWFString& string)
			{
				m_length = string.length();
				if (!m_length) return;

				char* data_ptr = wk::Memory::allocate<char>(m_length + 1);
				m_data = wk::Ref<char>(data_ptr);

				wk::Memory::copy(string.data(), data_ptr, m_length + 1);
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
				return m_data.get();
			}

			std::string string() const
			{
				if (empty()) return std::string();
				return std::string((const char*)m_data.get(), (const char*)m_data.get() + m_length);
			}

			void clear()
			{
				if (m_data)
				{
					m_data.reset();
				}
				m_length = 0;
			}

			int compare(const char* string, size_t len = 0) const
			{
				uint8_t string_size = static_cast<uint8_t>(len);
				if (string && string_size == 0)
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
					const char* rsymbol = m_data.get() + i;

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

				if (new_length >= 0xFF)
				{
					new_length = 0xFE;
				}

				if (new_length == 0)
				{
					clear();
					return;
				}

				char* new_data = wk::Memory::allocate<char>(new_length + 1);

				for (uint8_t i = 0; new_length > i; i++)
				{
					*(new_data + i) = i >= m_length ? fill : *(m_data.get() + i);
				}
				*(new_data + new_length) = '\0';

				clear();
				m_data = wk::Ref<char>(new_data);
				m_length = new_length;
			}

			bool operator==(const SWFString& other) const
			{
				return compare(other.data(), other.length()) == 0;
			}

			bool operator==(const char* other) const
			{
				return compare(other, strlen(other)) == 0;
			}

		public:
			// Function for character filtering
			// Since strings in sc in ASCII and are quite sensitive, we need to carefully select only required character range
			static bool IsValid(char ch) {
				return (ch >= 32 && ch <= 126) || ch == '\n' || ch == '\t';
			}

		private:
			wk::Ref<char> m_data = nullptr;
			uint8_t m_length = 0;
		};
	}
}

namespace wk::hash
{
	template<>
	struct Hash_t<sc::flash::SWFString>
	{
		template<typename T>
		static void update(HashStream<T>& stream, const sc::flash::SWFString& string)
		{
			if (string.empty()) return;

			stream.update((const uint8_t*)string.data(), string.length());
		}
	};
}