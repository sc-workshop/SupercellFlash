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
			virtual ~ColorTransform() = default;

		public:
			uint8_t alpha = 255;

			ColorRGB add{ 0, 0, 0 };
			ColorRGB multiply{ 255, 255, 255 };

		public:
			virtual void load(SupercellSWF& swf);
			virtual void save(SupercellSWF& swf) const;

			virtual uint8_t tag(SupercellSWF& swf) const;

		public:
			bool operator==(const ColorTransform& color) const;
		};
	}
}
