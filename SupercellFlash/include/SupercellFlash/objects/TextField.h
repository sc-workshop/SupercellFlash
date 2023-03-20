#pragma once

#include "SupercellFlash/objects/DisplayObject.h"

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
		uint32_t fontColor() { return m_fontColor; }
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
		bool isDynamic() { return m_isDynamic; }

		uint32_t outlineColor() { return m_outlineColor; }
		bool useDeviceFont() { return m_useDeviceFont; }
		bool autoAdjustFontSize() { return m_autoAdjustFontSize; }

		float bendAngle() { return m_bendAngle; }

		bool unknownFlag() const { return m_unknownFlag; }
		int16_t unknownShort() const { return m_unknownShort; }

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
		void isDynamic(bool status) { m_isDynamic = status; }

		void outlineColor(int32_t color) { m_outlineColor = color; }
		void useDeviceFont(bool status) { m_useDeviceFont = status; }
		void autoAdjustFontSize(bool status) { m_autoAdjustFontSize = status; }

		void bendAngle(float bend) { m_bendAngle = bend; }

		void unknownFlag(bool status) { m_unknownFlag = status; }
		void unknownShort(int16_t value) { m_unknownShort = value; }

	public:
		void load(SupercellSWF* swf, uint8_t tag);
		void save(SupercellSWF* swf);

		bool isTextField() const override { return true; }

	private:
		std::string m_text = "";

		std::string m_fontName = "";
		uint32_t m_fontColor = 0xFFFFFFFF;
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
		bool m_isDynamic = false;

		uint32_t m_outlineColor = 0xFFFFFFFF;
		bool m_useDeviceFont = true;
		bool m_autoAdjustFontSize = false;

		float m_bendAngle = 0.0f;

		bool m_unknownFlag = false;
		uint16_t m_unknownShort = 0xFFFF;
		uint16_t m_unknownShort2 = 0xFFFF;
	};
}
