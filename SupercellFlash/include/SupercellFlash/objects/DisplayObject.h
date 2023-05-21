#pragma once

#include <string>
#include <cstdint>

namespace sc
{
	class DisplayObject
	{
	public:
		uint16_t id() const { return m_id; }
		void id(uint16_t id) { m_id = id; }

	protected:
		uint16_t m_id = 0;
	};
}
