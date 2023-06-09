#pragma once

#include <string>
#include <cstdint>
#include <memory>

using namespace std;

namespace sc
{
	class SupercellSWF;

	struct MovieClipFrame
	{
	public:
		string label() { return m_label; }
		uint16_t elementsCount() { return m_elementsCount; }

	public:
		void label(string name) { m_label = name; }
		void elementsCount(uint16_t count) { m_elementsCount = count; }

	public:
		MovieClipFrame* load(SupercellSWF* swf);
		void save(SupercellSWF* movieClipStream);

	private:
		uint16_t m_elementsCount = 0;
		string m_label = "";

		uint8_t getTag();
	};

	typedef std::shared_ptr<MovieClipFrame> pMovieClipFrame;
}
