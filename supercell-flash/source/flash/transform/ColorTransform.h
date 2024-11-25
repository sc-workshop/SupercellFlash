#pragma once

#include <core/math/color_rgb.h>

namespace sc
{
	namespace flash {
		class SupercellSWF;

		struct ColorTransform
		{
		public:
			ColorTransform() {};

		public:
			uint8_t alpha = 255;

			wk::ColorRGB add{ 0, 0, 0 };
			wk::ColorRGB multiply{ 255, 255, 255 };

		public:
			void load(SupercellSWF& swf);
			void save(SupercellSWF& swf) const;

			uint8_t tag(SupercellSWF& swf) const;

		public:
			bool operator==(const ColorTransform& color) const;
		};
	}
}
