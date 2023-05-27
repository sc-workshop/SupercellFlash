#include "SupercellFlash/SupercellSWF.h"
#include "SupercellFlash/objects/TextField.h"

using namespace std;

namespace sc
{
	TextField* TextField::load(SupercellSWF* swf, uint8_t tag)
	{
		m_id = swf->stream.readUnsignedShort();

		m_fontName = swf->stream.readAscii();
		m_fontColor = swf->stream.readInt();

		m_isBold = swf->stream.readBool();
		m_isItalic = swf->stream.readBool();
		m_isMultiline = swf->stream.readBool();
		m_isDynamic = swf->stream.readBool();

		uint8_t fontSets = swf->stream.readUnsignedByte();
		if ((fontSets & 1) != 0) {
			m_fontAlign = Align::Right;
		}
		if ((fontSets & 2) != 0) {
			m_fontAlign = Align::Center;
		}
		if ((fontSets & 4) != 0) {
			m_fontAlign = Align::Justify;
		}

		m_fontSets = fontSets; // just for some time

		m_fontSize = swf->stream.readUnsignedByte();

		m_left = swf->stream.readShort();
		m_top = swf->stream.readShort();
		m_right = swf->stream.readShort();
		m_bottom = swf->stream.readShort();

		m_isOutlined = swf->stream.readBool();

		m_text = swf->stream.readAscii();

		if (tag == TAG_TEXT_FIELD)
			return this;

		m_useDeviceFont = swf->stream.readBool();

		if (tag > TAG_TEXT_FIELD_2)
			m_unknownFlag = (tag != 25);

		if (tag > TAG_TEXT_FIELD_3)
			m_outlineColor = swf->stream.readInt();

		if (tag > TAG_TEXT_FIELD_5)
		{
			m_unknownShort = swf->stream.readShort();
			m_unknownShort2 = swf->stream.readShort(); // unused
		}

		if (tag > TAG_TEXT_FIELD_6)
			m_bendAngle = swf->stream.readShort() * 91.019f;

		if (tag > TAG_TEXT_FIELD_7)
			m_autoAdjustFontSize = swf->stream.readBool();

		return this;
	}

	void TextField::save(SupercellSWF* swf)
	{
		uint8_t tag = TAG_TEXT_FIELD;

		uint32_t pos = swf->stream.initTag();

		swf->stream.writeUnsignedShort(m_id);

		swf->stream.writeAscii(m_fontName);
		swf->stream.writeInt(m_fontColor);

		swf->stream.writeBool(m_isBold);
		swf->stream.writeBool(m_isItalic);
		swf->stream.writeBool(m_isMultiline);
		swf->stream.writeBool(m_isDynamic);

		uint8_t fontSets = 0;

		switch (m_fontAlign)
		{
		case sc::TextField::Align::Right:
			fontSets |= (1 << 0);
			break;
		case sc::TextField::Align::Center:
			fontSets |= (1 << 1);
			break;
		case sc::TextField::Align::Justify:
			fontSets |= (1 << 2);
			break;
		case sc::TextField::Align::Left:
		default:
			break;
		}

		bool bool3 = (m_fontSets & 8) != 0;
		bool bool4 = (m_fontSets & 16) != 0;
		bool bool5 = (m_fontSets & 32) != 0;
		bool bool6 = (m_fontSets & 64) != 0;
		bool bool7 = (m_fontSets & 128) != 0;

		if (bool3) {
			fontSets |= (1 << 3);
		}
		if (bool4) {
			fontSets |= (1 << 4);
		}
		if (bool5) {
			fontSets |= (1 << 5);
		}
		if (bool6) {
			fontSets |= (1 << 6);
		}
		if (bool7) {
			fontSets |= (1 << 7);
		}

		swf->stream.writeUnsignedByte(fontSets);
		swf->stream.writeUnsignedByte(m_fontSize);

		swf->stream.writeShort(m_left);
		swf->stream.writeShort(m_top);
		swf->stream.writeShort(m_right);
		swf->stream.writeShort(m_bottom);

		swf->stream.writeUnsignedByte(m_isOutlined);

		swf->stream.writeAscii(m_text);

		if (m_useDeviceFont)
			tag = TAG_TEXT_FIELD_2;

		if (m_unknownFlag)
			tag = TAG_TEXT_FIELD_3;

		if (m_outlineColor != 0xFFFFFFFF)
			tag = TAG_TEXT_FIELD_4;

		if (m_unknownShort != 0xFFFF || m_unknownShort2 != 0xFFFF)
			tag = TAG_TEXT_FIELD_6;

		if (m_bendAngle != 0.0f)
			tag = TAG_TEXT_FIELD_7;

		if (m_autoAdjustFontSize)
			tag = TAG_TEXT_FIELD_8;

		if (m_unknownFlag2)
			tag = TAG_TEXT_FIELD_9;

		if (tag == TAG_TEXT_FIELD) goto FINALIZE;

		swf->stream.writeBool(m_useDeviceFont);

		if (tag == TAG_TEXT_FIELD_2 || tag == TAG_TEXT_FIELD_3) goto FINALIZE;

		swf->stream.writeInt(m_outlineColor);

		if (tag == TAG_TEXT_FIELD_4 || tag == TAG_TEXT_FIELD_5) goto FINALIZE;

		swf->stream.writeShort(m_unknownShort);
		swf->stream.writeShort(m_unknownShort2);

		if (tag == TAG_TEXT_FIELD_6) goto FINALIZE;

		swf->stream.writeShort((int16_t)(m_bendAngle / 91.019f));

		if (tag == TAG_TEXT_FIELD_7) goto FINALIZE;

		swf->stream.writeBool(m_autoAdjustFontSize);

		if (tag == TAG_TEXT_FIELD_8) goto FINALIZE;
		
		swf->stream.writeBool(m_unknownFlag2);

FINALIZE:
		swf->stream.finalizeTag(tag, pos);
	}
}