#pragma once

#include <string>
#include <cstdint>

using namespace std;

namespace sc {
	struct ExportName
	{
		string name() {
			return m_name;
		}
		uint16_t id() {
			return m_id;
		}

		void name(const string name) {
			m_name = name;
		}
		void id(uint16_t id) {
			m_id = id;
		}

	private:
		string m_name;
		uint16_t m_id;
	};

	typedef shared_ptr<ExportName> pExportName;
}
