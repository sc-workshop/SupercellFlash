#pragma once

#include "DisplayObject.h"
#include "flash/types/SWFString.hpp"

#include "flash/SC2/DataStorage_generated.h"
#include "flash/SC2/TextFields_generated.h"

#include "core/math/color_rgba.h"

namespace sc
{
	namespace flash {
		class SupercellSWF;

		class TextField : public DisplayObject
		{
		public:
			TextField() {};
			virtual ~TextField() = default;
			TextField(const TextField&) = default;
			TextField(TextField&&) = default;
			TextField& operator=(const TextField&) = default;
			TextField& operator=(TextField&&) = default;

		public:
			enum class Align : uint8_t
			{
				Right,
				Center,
				Justify,
				Left,
			};

			static Align get_horizontal_align(uint8_t flags);
			static Align get_vertical_align(uint8_t flags);

		public:
			SWFString text = "";

			SWFString font_name = "";
			wk::ColorRGBA font_color = {0xFF, 0xFF, 0xFF};
			uint8_t font_size = 0;
			Align font_horizontal_align = Align::Left;
			Align font_vertical_align = Align::Left;
			bool unknown_align6 = false;
			bool unknown_align7 = false;

			int16_t left = 0;
			int16_t top = 0;
			int16_t right = 0;
			int16_t bottom = 0;

			bool is_bold = false;
			bool is_italic = false;
			bool is_multiline = false;
			bool is_outlined = false;
			bool unknown_flag3 = false;

			uint32_t outline_color = 0x000000FF;
			bool use_device_font = false;
			bool auto_kern = false;

			float bend_angle = 0.0f;

			bool unknown_flag = false;
			uint16_t unknown_short = 0xFFFF;
			uint16_t unknown_short2 = 0xFFFF;

			SWFString typography_file = "";

		public:
			virtual void load(SupercellSWF& swf, uint8_t tag);
			virtual void save(SupercellSWF& swf) const;
			virtual void save_data(SupercellSWF& swf, uint8_t tag) const;

			virtual uint8_t tag(SupercellSWF& swf) const;

			virtual bool is_textfield() const;

			uint8_t get_style_flags() const;
			void set_style_flags(uint8_t style);

			uint8_t get_align_flags() const;
			void set_align_flags(uint8_t style);

		public:
			static void load_sc2(SupercellSWF&, const SC2::DataStorage*, const uint8_t*);
		};
	}
}
