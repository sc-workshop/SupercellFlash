#pragma once

#include <stdint.h>

#include "math/rect.h"

#include "SupercellFlash/types/SWFString.hpp"
#include "SupercellFlash/types/SWFContainer.hpp"

#include "SupercellFlash/objects/DisplayObject.h"
#include "SupercellFlash/objects/MovieClipFrame.h"

namespace sc
{
	class SupercellSWF;

	struct MovieClipFrameElement
	{
		uint16_t instance_index;
		uint16_t matrix_index = 0xFFFF;
		uint16_t colorTransform_index = 0xFFFF;
	};

	struct DisplayObjectInstance
	{
		enum class BlendMode : uint8_t
		{
			Normal = 0,
			// Normal1 = 1,
			Layer = 2,
			Multiply,
			Screen,
			Lighten,
			Darken,
			Difference,
			Add,
			Subtract,
			Invert,
			Alpha,
			Erase,
			Overlay,
			HardLight,
		};

		uint16_t id;
		BlendMode blend_mode = BlendMode::Normal;
		SWFString name;
	};

	class MovieClip : public DisplayObject
	{
	public:
		MovieClip() {};
		virtual ~MovieClip() = default;

	public:
		SWFVector<MovieClipFrameElement, uint32_t> frame_elements;
		SWFVector<DisplayObjectInstance> instances;
		SWFVector<MovieClipFrame> frames;

	public:
		uint8_t frame_rate = 24;

		uint8_t bank_index = 0;

		bool use_nine_slice = false;
		Rect<float> scaling_grid;

		uint8_t custom_property = 0;

		bool unknown_flag = false;

	public:
		virtual void load(SupercellSWF& swf, uint8_t tag);
		virtual void save(SupercellSWF& swf) const;

		virtual uint8_t tag(SupercellSWF& swf) const;

		virtual bool is_movieclip() const;
	};
}
