#pragma once

#include <string>

#include "SupercellFlash/flash/objects/DisplayObject.h"
#include "SupercellFlash/math/Rect.h"

namespace sc
{
	class SupercellSWF;
	class TextField : public DisplayObject
	{
	public:
		TextField() { }
		virtual ~TextField() { }

	public:
		std::string fontName() const { return m_fontName; }
		void fontName(const std::string& fontName) { m_fontName = fontName; }

		int32_t fontColor() const { return m_fontColor; }
		void fontColor(int32_t fontColor) { m_fontColor = fontColor; }

		int32_t outlineColor() const { return m_outlineColor; }
		void outlineColor(int32_t outlineColor) { m_outlineColor = outlineColor; }

		uint8_t align() const { return m_align; }
		void align(uint8_t align) { m_align = align; }

		uint8_t fontSize() const { return m_fontSize; }
		void fontSize(uint8_t fontSize) { m_fontSize = fontSize; }

		int16_t left() const { return m_left; }
		void left(int16_t left) { m_left = left; }

		int16_t top() const { return m_top; }
		void top(int16_t top) { m_top = top; }

		int16_t right() const { return m_right; }
		void right(int16_t right) { m_right = right; }

		int16_t bottom() const { return m_bottom; }
		void bottom(int16_t bottom) { m_bottom = bottom; }

		Rect& boundingBox() { return Rect(m_left, m_top, m_right, m_bottom); }
		void boundingBox(const Rect& bbox)
		{
			m_left = bbox.x;
			m_top = bbox.y;
			m_right = bbox.width;
			m_bottom = bbox.height;
		}

		bool isBold() const { return m_isBold; }
		void isBold(bool status) { m_isBold = status; }

		bool isItalic() const { return m_isItalic; }
		void isItalic(bool status) { m_isItalic = status; }

		bool isMultiline() const { return m_isMultiline; }
		void isMultiline(bool status) { m_isMultiline = status; }

		bool isOutlineEnabled() const { return m_isOutlineEnabled; }
		void isOutlineEnabled(bool status) { m_isOutlineEnabled = status; }

		bool usingDeviceFont() const { return m_usingDeviceFont; }
		void usingDeviceFont(bool status) { m_usingDeviceFont = status; }

		bool autoAdjustFontSize() const { return m_autoAdjustFontSize; }
		void autoAdjustFontSize(bool status) { m_autoAdjustFontSize = status; }

		float bendAngle() const { return m_bendAngle; }
		void bendAngle(float angle) { m_bendAngle = angle; }

		std::string text() const { return m_text; }
		void text(const std::string& text) { m_text = text; }

		bool unknownFlag() const { return m_unknownFlag; }
		void unknownFlag(bool status) { m_unknownFlag = status; }

		int16_t unknownShort() const { return m_unknownShort; }
		void unknownShort(int16_t value) { m_unknownShort = value; }

		bool isTextField() const override { return true; }

	public:
		void load(SupercellSWF* swf, uint8_t tag);

	private:
		std::string m_fontName = "";

		int32_t m_fontColor = -1;
		int32_t m_outlineColor = -1;

		uint8_t m_align = 0;
		uint8_t m_fontSize = 0;

		int16_t m_left = 0;
		int16_t m_top = 0;
		int16_t m_right = 0;
		int16_t m_bottom = 0;

		bool m_isBold = false;
		bool m_isItalic = false;
		bool m_isMultiline = false;
		bool m_isOutlineEnabled = false;
		bool m_usingDeviceFont = false;
		bool m_autoAdjustFontSize = false;

		float m_bendAngle = 0.0f;

		std::string m_text = "";

		bool m_unknownFlag = false; // only if tag != 25
		int16_t m_unknownShort = 0; // appears in tag >= 33
	};
}
