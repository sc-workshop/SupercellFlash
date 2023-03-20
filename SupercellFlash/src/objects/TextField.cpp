#include "SupercellFlash/SupercellSWF.h"
#include "SupercellFlash/objects/TextField.h"

namespace sc
{
	void TextField::load(SupercellSWF* swf, uint8_t tag)
	{
		m_id = swf->stream.readUnsignedShort();

		m_fontName = swf->stream.readAscii();
		m_fontColor = swf->stream.readInt();

		m_isBold = swf->stream.readBool();
		m_isItalic = swf->stream.readBool();
		m_isMultiline = swf->stream.readBool();
		m_isDynamic = swf->stream.readBool();

		m_fontAlign = swf->stream.readUnsignedByte();
		m_fontSize = swf->stream.readUnsignedByte();

		m_left = swf->stream.readShort();
		m_top = swf->stream.readShort();
		m_right = swf->stream.readShort();
		m_bottom = swf->stream.readShort();

		m_isOutlined = swf->stream.readBool();

		m_text = swf->stream.readAscii();

		if (tag == TAG_TEXT_FIELD)
			return;

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

		swf->stream.writeUnsignedByte(m_fontAlign);
		swf->stream.writeUnsignedByte(m_fontSize);

		swf->stream.writeShort(m_left);
		swf->stream.writeShort(m_top);
		swf->stream.writeShort(m_right);
		swf->stream.writeShort(m_bottom);

		swf->stream.writeUnsignedByte(m_isOutlined);

		swf->stream.writeAscii(m_text);

		if (!m_useDeviceFont)
			tag = TAG_TEXT_FIELD_2;

		if (m_outlineColor != 0xFFFFFFFF) {
			tag = TAG_TEXT_FIELD_4;
		}

		if (m_unknownShort != 0xFFFF || m_unknownShort2 != 0xFFFF) {
			tag = TAG_TEXT_FIELD_6;
		}

		if (m_bendAngle != 0.0f) {
			tag = TAG_TEXT_FIELD_7;
		}

		if (m_autoAdjustFontSize) {
			tag = TAG_TEXT_FIELD_8;
		}

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

FINALIZE:
		swf->stream.finalizeTag(tag, pos);
	}
}