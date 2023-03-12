#pragma once

#include <string>
#include <cstdint>

namespace sc
{
	class SupercellSWF;

	struct MovieClipFrame
	{
		uint16_t elementsCount;
		std::string label;

		void load(SupercellSWF* swf);
		void save(SupercellSWF* movieClipStream);
	};
}
