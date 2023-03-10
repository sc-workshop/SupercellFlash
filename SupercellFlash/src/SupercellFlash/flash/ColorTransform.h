#pragma once

#include <cstdint>

namespace sc
{
	class SupercellSWF;

	struct ColorTransform
	{
		uint8_t redAddition = 0;
		uint8_t greenAddition = 0;
		uint8_t blueAddition = 0;

		uint8_t redMultiplier = 255;
		uint8_t greenMultiplier = 255;
		uint8_t blueMultiplier = 255;

		uint8_t alpha = 255;

		void load(SupercellSWF* swf);
	};
}
