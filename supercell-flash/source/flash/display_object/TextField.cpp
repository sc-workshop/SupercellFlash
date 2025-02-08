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
			font_color.set_value(swf.stream.read_unsigned_int());

			is_bold = swf.stream.read_bool();
			is_italic = swf.stream.read_bool();
			is_multiline = swf.stream.read_bool();
			unknown_flag3 = swf.stream.read_bool();

			set_align_flags(swf.stream.read_unsigned_byte());

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
			swf.stream.write_unsigned_int(font_color.as_value());

			swf.stream.write_bool(is_bold);
			swf.stream.write_bool(is_italic);
			swf.stream.write_bool(is_multiline);
			swf.stream.write_bool(unknown_flag3);

			swf.stream.write_unsigned_byte(get_align_flags());
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

		TextField::Align TextField::get_horizontal_align(uint8_t flags)
		{
			if ((flags & 1) != 0) {
				return Align::Right;
			}
			if ((flags & (1 << 1)) != 0) {
				return Align::Center;
			}
			if ((flags & (1 << 2)) != 0) {
				return Align::Justify;
			}

			return Align::Left;
		}

		TextField::Align TextField::get_vertical_align(uint8_t flags)
		{
			if ((flags & (1 << 3)) != 0) {
				return Align::Right;
			}
			if ((flags & (1 << 4)) != 0) {
				return Align::Center;
			}
			if ((flags & (1 << 5)) != 0) {
				return Align::Justify;
			}

			return Align::Left;
		}

		bool TextField::is_textfield() const
		{
			return true;
		}

		void TextField::load_sc2(SupercellSWF& swf, const SC2::DataStorage* storage, const uint8_t* data)
		{
			using Style = SC2::TextFieldStyle;
			auto textfields_data = SC2::GetTextFields(data);

			auto textfields_vector = textfields_data->textfields();
			if (!textfields_vector) return;

			auto strings_vector = storage->strings();
			uint16_t textfields_count = (uint16_t)textfields_vector->size();
			swf.textfields.reserve(textfields_count);

			for (uint16_t i = 0; textfields_count > i; i++)
			{
				auto textfield_data = textfields_vector->Get(i);
				TextField& textfield = swf.textfields[i];

				textfield.id = textfield_data->id();
				textfield.font_name = SWFString(
					strings_vector->Get(
						textfield_data->font_name_ref_id()
					)->c_str()
				);

				textfield.set_style_flags(textfield_data->styles());

				textfield.left = textfield_data->left();
				textfield.right = textfield_data->right();
				textfield.top = textfield_data->top();
				textfield.bottom = textfield_data->bottom();

				textfield.font_color.set_value(textfield_data->font_color());
				textfield.outline_color = textfield_data->outline_color();

				textfield.text = SWFString(
					strings_vector->Get(
						textfield_data->text_ref_id()
					)->c_str()
				);

				textfield.typography_file = SWFString(
					strings_vector->Get(
						textfield_data->typography_ref_id()
					)->c_str()
				);

				textfield.set_align_flags(textfield_data->align());

				textfield.font_size = textfield_data->font_size();
				textfield.unknown_short = textfield_data->unknown_short();
			}
		}

		uint8_t TextField::get_style_flags() const
		{
			using Style = SC2::TextFieldStyle;

			uint8_t result = 0;

			if (is_bold) result |= (uint8_t)Style::bold;
			if (is_italic) result |= (uint8_t)Style::italic;
			if (is_multiline) result |= (uint8_t)Style::is_multiline;
			if (is_outlined) result |= (uint8_t)Style::has_outline;
			if (unknown_flag3) result |= (uint8_t)Style::unknown_flag3;
			if (unknown_flag) result |= (uint8_t)Style::unknown_flag;
			if (auto_kern) result |= (uint8_t)Style::auto_kern;

			return result;
		}

		void TextField::set_style_flags(uint8_t style)
		{
			using Style = SC2::TextFieldStyle;

			is_bold = (style & (uint8_t)Style::bold) > 0;
			is_italic = (style & (uint8_t)Style::italic) > 0;
			is_multiline = (style & (uint8_t)Style::is_multiline) > 0;
			is_outlined = (style & (uint8_t)Style::has_outline) > 0;
			unknown_flag3 = (style & (uint8_t)Style::unknown_flag3) > 0;
			unknown_flag = (style & (uint8_t)Style::unknown_flag) > 0;
			auto_kern = (style & (uint8_t)Style::auto_kern) > 0;
		}

		uint8_t TextField::get_align_flags() const
		{
			uint8_t result = 0;
			if (font_horizontal_align != Align::Left)
			{
				result |= (1 << (uint8_t)font_horizontal_align);
			}

			if (font_vertical_align != Align::Left)
			{
				result |= ((1 << 3) << (uint8_t)font_vertical_align);
			}

			result |= (unknown_align6 << 6);
			result |= (unknown_align7 << 7);

			return result;
		}

		void TextField::set_align_flags(uint8_t flags)
		{
			font_horizontal_align = TextField::get_horizontal_align(flags);
			font_vertical_align = TextField::get_vertical_align(flags);

			unknown_align6 = (flags & (1 << 6)) != 0;
			unknown_align7 = (flags & (1 << 7)) != 0;
		}
	}
}