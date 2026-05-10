#include "TextField.h"

#include "flash/objects/SupercellSWF.h"

namespace sc::flash {
    void TextField::load(SupercellSWF& swf, uint8_t tag) {
        id = swf.stream.read_unsigned_short();

        swf.stream.read_string(font_name);
        font_color.set_value(swf.stream.read_unsigned_int());

        if (swf.stream.read_bool())
            style.set_bold(true);

        if (swf.stream.read_bool())
            style.set_italic(true);

        if (swf.stream.read_bool())
            style.set_multiline(true);

        swf.stream.read_bool(); // Unused

        set_align_flags(swf.stream.read_unsigned_byte());

        font_size = swf.stream.read_unsigned_byte();

        left = swf.stream.read_short();
        top = swf.stream.read_short();
        right = swf.stream.read_short();
        bottom = swf.stream.read_short();

        if (swf.stream.read_bool())
            style.set_outlined(true);

        swf.stream.read_string(text);

        if (tag == TAG_TEXT_FIELD)
            return;

        if (swf.stream.read_bool())
            style.set_use_device_font(true);

        switch (tag) {
            case TAG_TEXT_FIELD_2:
                return;
            case TAG_TEXT_FIELD_3:
                style.set_unknown_flag(true);
                return;
            case TAG_TEXT_FIELD_4:
                outline_color.set_value(swf.stream.read_unsigned_int());
                style.set_unknown_flag(true);
                return;
            case TAG_TEXT_FIELD_5:
                outline_color.set_value(swf.stream.read_unsigned_int());
                return;
            case TAG_TEXT_FIELD_6:
            case TAG_TEXT_FIELD_7:
            case TAG_TEXT_FIELD_8:
            case TAG_TEXT_FIELD_9:
                outline_color.set_value(swf.stream.read_unsigned_int());
                outline_angle = (swf.stream.read_short() & 0xFFFF) | ((swf.stream.read_short() & 0xFFFF) << 16);
                style.set_unknown_flag(true);

                if (tag == TAG_TEXT_FIELD_6)
                    return;

                bend_angle = (std::numeric_limits<int16_t>::max() * swf.stream.read_short()) / 360.f;
                if (tag == TAG_TEXT_FIELD_7)
                    return;

                if (swf.stream.read_bool())
                    style.set_auto_kern(true);

                if (tag == TAG_TEXT_FIELD_8)
                    return;

                swf.stream.read_string(typography_file);
        }
    }

    void TextField::save(SupercellSWF& swf) const {
        swf.stream.write_unsigned_short(id);

        swf.stream.write_string(font_name);
        swf.stream.write_unsigned_int(font_color.as_value());

        swf.stream.write_bool(style.is_bold());
        swf.stream.write_bool(style.is_italic());
        swf.stream.write_bool(style.is_multiline());
        swf.stream.write_bool(false);

        swf.stream.write_unsigned_byte(get_align_flags());
        swf.stream.write_unsigned_byte(font_size);

        swf.stream.write_short(left);
        swf.stream.write_short(top);
        swf.stream.write_short(right);
        swf.stream.write_short(bottom);

        swf.stream.write_bool(style.is_outlined());

        swf.stream.write_string(text);

        save_data(swf, tag(swf));
    }

    uint8_t TextField::tag(SupercellSWF&) const {
        uint8_t tag = TAG_TEXT_FIELD;

        if (style.use_device_font())
            tag = TAG_TEXT_FIELD_2;

        if (style.unknown_flag())
            tag = TAG_TEXT_FIELD_3;

        if (style.is_outlined() && outline_color.as_value() != 0)
            tag = TAG_TEXT_FIELD_5;

        if (outline_angle != int32_t(0xFFFF0000))
            tag = TAG_TEXT_FIELD_6;

        if (bend_angle != 0.0f)
            tag = TAG_TEXT_FIELD_7;

        if (style.auto_kern())
            tag = TAG_TEXT_FIELD_8;

        if (!typography_file.empty())
            tag = TAG_TEXT_FIELD_9;

        return tag;
    }

    void TextField::save_data(SupercellSWF& swf, uint8_t tag) const {
        if (tag == TAG_TEXT_FIELD)
            return;

        swf.stream.write_bool(style.use_device_font());

        if (tag == TAG_TEXT_FIELD_2 || tag == TAG_TEXT_FIELD_3)
            return;

        swf.stream.write_unsigned_int(outline_color.as_value());

        if (tag == TAG_TEXT_FIELD_4 || tag == TAG_TEXT_FIELD_5)
            return;

        swf.stream.write_short(outline_angle & 0xFFFF);
        swf.stream.write_short((((outline_angle >> 16) & 0xFFFF) * 360) / 32767);

        if (tag == TAG_TEXT_FIELD_6)
            return;

        swf.stream.write_short((int16_t) (360.f * bend_angle / std::numeric_limits<int16_t>::max()));

        if (tag == TAG_TEXT_FIELD_7)
            return;

        swf.stream.write_bool(style.auto_kern());

        if (tag == TAG_TEXT_FIELD_8)
            return;

        swf.stream.write_string(typography_file);
    }

    TextField::Align TextField::get_horizontal_align(uint8_t flags) {
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

    TextField::Align TextField::get_vertical_align(uint8_t flags) {
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

    bool TextField::is_textfield() const {
        return true;
    }

    void TextField::load_sc2(SupercellSWF& swf, const SC2::DataStorage* storage, const uint8_t* data) {
        auto textfields_data = SC2::GetTextFields(data);

        auto textfields_vector = textfields_data->textfields();
        if (!textfields_vector)
            return;

        auto strings_vector = storage->strings();
        uint16_t textfields_count = (uint16_t) textfields_vector->size();
        swf.textfields.reserve(textfields_count);

        for (uint16_t i = 0; textfields_count > i; i++) {
            auto textfield_data = textfields_vector->Get(i);
            TextField& textfield = swf.textfields[i];

            textfield.id = textfield_data->id();
            textfield.font_name = SWFString(strings_vector->Get(textfield_data->font_name_ref_id())->c_str());

            textfield.style = TextFieldStyle(textfield_data->styles());

            textfield.left = textfield_data->left();
            textfield.right = textfield_data->right();
            textfield.top = textfield_data->top();
            textfield.bottom = textfield_data->bottom();

            textfield.font_color.set_value(textfield_data->font_color());

            auto value = textfield_data->outline_color();
            textfield.outline_color.set_value(value);

            textfield.text = SWFString(strings_vector->Get(textfield_data->text_ref_id())->c_str());

            textfield.typography_file = SWFString(strings_vector->Get(textfield_data->typography_ref_id())->c_str());

            textfield.set_align_flags(textfield_data->align());

            textfield.font_size = textfield_data->font_size();
            textfield.outline_angle = textfield_data->outline_angle();
        }
    }

    uint8_t TextField::get_align_flags() const {
        uint8_t result = 0;
        if (font_horizontal_align != Align::Left) {
            result |= (1 << (uint8_t) font_horizontal_align);
        }

        if (font_vertical_align != Align::Left) {
            result |= ((1 << 3) << (uint8_t) font_vertical_align);
        }

        result |= (unknown_align6 << 6);
        result |= (unknown_align7 << 7);

        return result;
    }

    void TextField::set_align_flags(uint8_t flags) {
        font_horizontal_align = TextField::get_horizontal_align(flags);
        font_vertical_align = TextField::get_vertical_align(flags);

        unknown_align6 = (flags & (1 << 6)) != 0;
        unknown_align7 = (flags & (1 << 7)) != 0;
    }
}
