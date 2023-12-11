#pragma once

#include <stdint.h>
#include "math/color.h"

namespace sc
{
	class SupercellSWF;

	struct ColorTransform
	{
	public:
		uint8_t alpha = 255;

		Color<uint8_t> add{ 0, 0, 0 };
		Color<uint8_t> multiply{ 255, 255, 255 };

	public:
		void load(SupercellSWF& swf);
		void save(SupercellSWF& swf) const;

		uint8_t tag() const;

	public:
		bool operator==(const ColorTransform& color) const;
	};
}
