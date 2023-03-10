#pragma once

#include <string>
#include <vector>

namespace sc
{
	class SupercellSWF;

	struct MovieClipFrameElement
	{
		uint16_t instanceIndex;
		uint16_t matrixIndex;
		uint16_t colorTransformIndex;
	};

	struct MovieClipFrame
	{
		std::string label;
		std::vector<MovieClipFrameElement> elements;

		uint16_t load(SupercellSWF* swf);
	};
}
