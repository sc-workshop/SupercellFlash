#pragma once

#include "DisplayObject.h"
#include "core/math/color_rgba.h"
#include "flash/SC2/DataStorage_generated.h"
#include "flash/SC2/TextFields_generated.h"
#include "flash/types/SWFString.hpp"

#include <bitset>

namespace sc::flash {
    class SupercellSWF;

    class TextFieldStyle {
    public:
        TextFieldStyle(uint16_t flags = 0) :
            m_flags(flags) {}

    public:
        void set_use_device_font(bool value) { m_flags.set(1, value); }
        bool use_device_font() const { return m_flags.test(1); }

        void set_outlined(bool value) { m_flags.set(1, value); }
        bool is_outlined() const { return m_flags.test(1); }

        void set_bold(bool value) { m_flags.set(2, value); }
        bool is_bold() const { return m_flags.test(2); }

        void set_italic(bool value) { m_flags.set(3, value); }
        bool is_italic() const { return m_flags.test(3); }

        void set_multiline(bool value) { m_flags.set(4, value); }
        bool is_multiline() const { return m_flags.test(4); }

        void set_unknown_flag(bool value) { m_flags.set(5, value); }
        bool unknown_flag() const { return m_flags.test(5); }

        void set_auto_kern(bool value) { m_flags.set(6, value); }
        bool auto_kern() const { return m_flags.test(6); }

        uint8_t flags() const { return (uint8_t) m_flags.to_ulong(); }
    private:
        std::bitset<8> m_flags;
    };

    class TextField : public DisplayObject {
    public:
        virtual ~TextField() = default;

    public:
        enum class Align : uint8_t {
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

        TextFieldStyle style;

        wk::ColorRGBA outline_color = {0x0, 0x0, 0x0, 0x0};

        float bend_angle = 0.0f;

        int32_t outline_angle = 0xFFFF0000;
        SWFString typography_file = "";

    public:
        virtual void load(SupercellSWF& swf, uint8_t tag);
        virtual void save(SupercellSWF& swf) const;
        virtual void save_data(SupercellSWF& swf, uint8_t tag) const;

        virtual uint8_t tag(SupercellSWF& swf) const;

        virtual bool is_textfield() const;

        uint8_t get_align_flags() const;
        void set_align_flags(uint8_t style);

    public:
        static void load_sc2(SupercellSWF&, const SC2::DataStorage*, const uint8_t*);
    };
}
