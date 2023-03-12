#pragma once

#include "SupercellFlash/flash/objects/DisplayObject.h"

namespace sc
{
	class SupercellSWF;

	class TextField : public DisplayObject
	{
	public:
		TextField() { }
		virtual ~TextField() { }

		/* Getters */
	public:
		std::string text() { return m_text; }

		std::string fontName() { return m_fontName; }
		int32_t fontColor() { return m_fontColor; }
		uint8_t fontSize() { return m_fontSize; }
		uint8_t fontAlign() { return m_fontAlign; }

		int16_t left() { return m_left; }
		int16_t top() { return m_top; }
		int16_t right() { return m_right; }
		int16_t bottom() { return m_bottom; }

		bool isBold() { return m_isBold; }
		bool isItalic() { return m_isItalic; }
		bool isMultiline() { return m_isMultiline; }
		bool isOutlined() { return m_isOutlined; }

		uint32_t outlineColor() { return m_outlineColor; }
		bool useDeviceFont() { return m_useDeviceFont; }
		bool autoAdjustFontBounds() { return m_autoAdjustFontBounds; }

		float bendAngle() { return m_bendAngle; }

		/* Setters */
	public:
		void text(const std::string& newText) { m_text = newText; }

		void fontName(const std::string& newfontName) { m_fontName = newfontName; }
		void fontColor(int32_t color) { m_fontColor = color; }
		void fontSize(uint8_t size) { m_fontSize = size; }
		void fontAlign(uint8_t align) { m_fontAlign = align; }

		void left(int16_t left) { m_left = left; }
		void top(int16_t top) { m_top = top; }
		void right(int16_t right) { m_right = right; }
		void bottom(int16_t bottom) { m_bottom = bottom; }

		void isBold(bool status) { m_isBold = status; }
		void isItalic(bool status) { m_isItalic = status; }
		void isMultiline(bool status) { m_isMultiline = status; }
		void isOutlined(bool status) { m_isOutlined = status; }

		void outlineColor(int16_t color) { m_outlineColor = color; }
		void useDeviceFont(bool status) { m_useDeviceFont = status; }
		void autoAdjustFontBounds(bool status) { m_autoAdjustFontBounds = status; }

		void bendAngle(float bend) { m_bendAngle = bend; }

	public:
		void load(SupercellSWF* swf, uint8_t tag);
		void save(SupercellSWF* swf);

	private:
		std::string m_text = "";

		std::string m_fontName = "";
		int32_t m_fontColor = 0xFFFFFF;
		uint8_t m_fontSize = 0;
		uint8_t m_fontAlign = 0;

		int16_t m_left = 0;
		int16_t m_top = 0;
		int16_t m_right = 0;
		int16_t m_bottom = 0;

		bool m_isBold = false;
		bool m_isItalic = false;
		bool m_isMultiline = false;
		bool m_isOutlined = false;

		int32_t m_outlineColor = 0xFFFFFF;
		bool m_useDeviceFont = false;
		bool m_autoAdjustFontBounds = false;

		float m_bendAngle = 0.0f;

		bool m_unknownFlag = false;
		int16_t m_unknownShort = 0;
	};
}
