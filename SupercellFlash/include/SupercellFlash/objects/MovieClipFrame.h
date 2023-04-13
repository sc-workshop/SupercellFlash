#pragma once

#include <string>
#include <cstdint>

namespace sc
{
	class SupercellSWF;

	struct MovieClipFrame
	{
	public:
		std::string label() { return m_label; }
		uint16_t elementsCount() { return m_elementsCount; }

	public:
		void label(std::string name) { m_label = name; }
		void elementsCount(uint16_t count) { m_elementsCount = count; }

	public:
		MovieClipFrame* load(SupercellSWF* swf);
		void save(SupercellSWF* movieClipStream);

	private:
		uint16_t m_elementsCount;
		std::string m_label;
	};
}
