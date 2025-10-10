#pragma once

#include <string>
#include <optional>
#include <algorithm>

#include "core/memory/memory.h"
#include "core/memory/ref.h"
#include "core/io/stream.h"
#include "core/io/memory_stream.h"
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

				size_t strLength = string_size >= 0xFE ? 0xFE : static_cast<uint8_t>(string_size);
				if (!strLength) return;

				m_data = std::string(data, strLength);
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

				size_t strLength = string.length() >= 0xFE ? 0xFE : static_cast<uint8_t>(string.length());
				if (!strLength) return;

				m_data = std::string(string.c_str(), strLength);
			}

			SWFString& operator=(const SWFString& string) = default;
			SWFString(const SWFString& string) = default;

		public:
			bool empty() const
			{
				return m_data.empty();
			}

			uint8_t length() const
			{
				return (uint8_t)m_data.length();
			}

			char* data() const
			{
				return (char*)m_data.data();
			}

			std::string string() const
			{
				return m_data;
			}

			void clear()
			{
				m_data.clear();
			}

			int compare(const char* string) const
			{
				return m_data.compare(0, std::string::npos, string);
			}

			int compare(const std::string& string) const
			{
				return m_data.compare(string);
			}

			void resize(uint8_t new_length, char fill = '\0')
			{
				if (length() == new_length)
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

				m_data.resize(new_length, fill);
			}

			bool operator==(const SWFString& other) const
			{
				return compare(other.data()) == 0;
			}

			bool operator==(const char* other) const
			{
				return compare(other) == 0;
			}

		public:
			// Function for character filtering
			// Since strings in sc in ASCII and are quite sensitive, we need to carefully select only required character range
			static bool IsValid(char ch) {
				return (ch >= 32 && ch <= 126) || ch == '\n' || ch == '\t';
			}

		private:
			std::string m_data;
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