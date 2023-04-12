#pragma once

#include <string>
#include <cstdint>

namespace sc {
	struct ExportName
	{
		std::string name() {
			return m_name;
		}
		uint16_t id() {
			return m_id;
		}

		void name(const std::string name) {
			m_name = name;
		}
		void id(uint16_t id) {
			m_id = id;
		}

	private:
		std::string m_name;
		uint16_t m_id;
	};
}
