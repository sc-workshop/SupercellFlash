#include "TextField.h"
#include "flash/objects/SupercellSWF.h"

using namespace std;

namespace sc
{
	namespace flash
	{
		void TextField::load(SupercellSWF& swf, uint8_t tag)
		{
			id = swf.stream.read_unsigned_short();

			swf.stream.read_string(font_name);
			font_color = swf.stream.read_unsigned_int();

			is_bold = swf.stream.read_bool();
			is_italic = swf.stream.read_bool();
			is_multiline = swf.stream.read_bool();
			unknown_flag3 = swf.stream.read_bool();

			uint8_t font_align_flags = swf.stream.read_unsigned_byte();
			if ((font_align_flags & 1) != 0) {
				font_horizontal_align = Align::Right;
			}
			if ((font_align_flags & (1 << 1)) != 0) {
				font_horizontal_align = Align::Center;
			}
			if ((font_align_flags & (1 << 2)) != 0) {
				font_horizontal_align = Align::Justify;
			}

			if ((font_align_flags & (1 << 3)) != 0) {
				font_vertical_align = Align::Right;
			}
			if ((font_align_flags & (1 << 4)) != 0) {
				font_vertical_align = Align::Center;
			}
			if ((font_align_flags & (1 << 5)) != 0) {
				font_vertical_align = Align::Justify;
			}

			unknown_align6 = (font_align_flags & (1 << 6)) != 0;
			unknown_align7 = (font_align_flags & (1 << 7)) != 0;

			font_size = swf.stream.read_unsigned_byte();

			left = swf.stream.read_short();
			top = swf.stream.read_short();
			right = swf.stream.read_short();
			bottom = swf.stream.read_short();

			is_outlined = swf.stream.read_bool();

			swf.stream.read_string(text);

			if (tag == TAG_TEXT_FIELD)
				return;

			use_device_font = swf.stream.read_bool();

			if (tag > TAG_TEXT_FIELD_2)
			{
				unknown_flag = (tag != 25);
			}

			if (tag > TAG_TEXT_FIELD_3)
			{
				outline_color = swf.stream.read_unsigned_int();
			}

			if (tag > TAG_TEXT_FIELD_5)
			{
				unknown_short = swf.stream.read_short();
				unknown_short2 = swf.stream.read_short();
			}

			if (tag > TAG_TEXT_FIELD_6)
			{
				bend_angle = swf.stream.read_short() * 91.019f;
			}

			if (tag > TAG_TEXT_FIELD_7)
			{
				auto_kern = swf.stream.read_bool();
			}

			if (tag > TAG_TEXT_FIELD_8)
			{
				swf.stream.read_string(typography_file);
			}
		}

		void TextField::save(SupercellSWF& swf) const
		{
			swf.stream.write_unsigned_short(id);

			swf.stream.write_string(font_name);
			swf.stream.write_unsigned_int(font_color);

			swf.stream.write_bool(is_bold);
			swf.stream.write_bool(is_italic);
			swf.stream.write_bool(is_multiline);
			swf.stream.write_bool(unknown_flag3);

			uint8_t font_align_flags = 0;
			if (font_horizontal_align != Align::Left)
			{
				font_align_flags |= (1 << (uint8_t)font_horizontal_align);
			}

			if (font_vertical_align != Align::Left)
			{
				font_align_flags |= ((1 << 3) << (uint8_t)font_vertical_align);
			}

			font_align_flags |= (unknown_align6 << 6);
			font_align_flags |= (unknown_align7 << 7);

			swf.stream.write_unsigned_byte(font_align_flags);
			swf.stream.write_unsigned_byte(font_size);

			swf.stream.write_short(left);
			swf.stream.write_short(top);
			swf.stream.write_short(right);
			swf.stream.write_short(bottom);

			swf.stream.write_bool(is_outlined);

			swf.stream.write_string(text);

			save_data(swf, tag(swf));
		}

		uint8_t TextField::tag(SupercellSWF&) const
		{
			uint8_t tag = TAG_TEXT_FIELD;

			if (use_device_font)
				tag = TAG_TEXT_FIELD_2;

			if (unknown_flag)
				tag = TAG_TEXT_FIELD_3;

			if (outline_color != 0x000000FF)
				tag = TAG_TEXT_FIELD_4;

			if (unknown_short != 0xFFFF || unknown_short2 != 0xFFFF)
				tag = TAG_TEXT_FIELD_6;

			if (bend_angle != 0.0f)
				tag = TAG_TEXT_FIELD_7;

			if (auto_kern)
				tag = TAG_TEXT_FIELD_8;

			if (!typography_file.empty())
				tag = TAG_TEXT_FIELD_9;

			return tag;
		}

		void TextField::save_data(SupercellSWF& swf, uint8_t tag) const
		{
			if (tag == TAG_TEXT_FIELD) return;

			swf.stream.write_bool(use_device_font);

			if (tag == TAG_TEXT_FIELD_2 || tag == TAG_TEXT_FIELD_3) return;

			swf.stream.write_unsigned_int(outline_color);

			if (tag == TAG_TEXT_FIELD_4 || tag == TAG_TEXT_FIELD_5) return;

			swf.stream.write_short(unknown_short);
			swf.stream.write_short(unknown_short2);

			if (tag == TAG_TEXT_FIELD_6) return;

			swf.stream.write_short((int16_t)(bend_angle / 91.019f));

			if (tag == TAG_TEXT_FIELD_7) return;

			swf.stream.write_bool(auto_kern);

			if (tag == TAG_TEXT_FIELD_8) return;

			swf.stream.write_string(typography_file);
		}

		bool TextField::is_textfield() const
		{
			return true;
		}
	}
}