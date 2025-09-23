#pragma once

#include <any>
#include <optional>

#include <core/math/rect.h>

#include "flash/types/SWFString.hpp"
#include "flash/types/SWFContainer.hpp"

#include "flash/display_object/DisplayObject.h"
#include "flash/display_object/MovieClipFrame.h"

#include "flash/SC2/DataStorage_generated.h"
#include "flash/SC2/MovieClips_generated.h"
#include "flash/SC2/CompressedMovieClips_generated.h"

namespace sc
{
	namespace flash {
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

		typedef SWFVector<MovieClipFrameElement, uint32_t> MovieClipFrameElementsArray;
		typedef SWFVector<DisplayObjectInstance> MovieClipChildrensArray;
		typedef SWFVector<MovieClipFrame> MovieClipFrameArray;

		class MovieClip : public DisplayObject
		{
		public:
			static inline size_t COMPRESSED_CLIP_DATA_MAX_SIZE = 4096;

		public:
			MovieClip() {};
			virtual ~MovieClip() = default;
			MovieClip(const MovieClip&) = default;
			MovieClip(MovieClip&&) = default;
			MovieClip& operator=(const MovieClip&) = default;
			MovieClip& operator=(MovieClip&&) = default;

		public:
			MovieClipFrameElementsArray frame_elements;
			MovieClipChildrensArray childrens;
			MovieClipFrameArray frames;

		public:
			uint8_t frame_rate = 24;

			uint32_t bank_index = 0;

			std::optional<wk::RectF> scaling_grid;

			SWFVector<std::any, uint8_t> custom_properties;

		public:
			bool unknown_flag = false;

		public:
			virtual void load(SupercellSWF& swf, uint8_t tag);
			virtual void save(SupercellSWF& swf) const;

			virtual uint8_t tag(SupercellSWF& swf) const;

			virtual bool is_movieclip() const;

			void write_frame_elements_buffer(wk::Stream& stream) const;

			static int decode_compressed_frame_data(uint16_t* m_pKeyframes, uint16_t* m_pData, uint16_t* m_pDataEnd, uint16_t* out);
		public:
			static void load_sc2(SupercellSWF&, const SC2::DataStorage*, const uint8_t*);
		private:
			static void load_compressed(SupercellSWF&, const SC2::DataStorage*, const uint8_t*);
		};
	}
}
