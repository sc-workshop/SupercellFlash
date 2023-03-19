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
			swf->stream.readShort(); // unused
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
		swf->stream.writeUnsignedByte(0); // unused

		swf->stream.writeUnsignedByte(m_fontAlign);
		swf->stream.writeUnsignedByte(m_fontSize);

		swf->stream.writeShort(m_left);
		swf->stream.writeShort(m_top);
		swf->stream.writeShort(m_right);
		swf->stream.writeShort(m_bottom);

		swf->stream.writeUnsignedByte(m_isOutlined);

		swf->stream.writeAscii(m_text);

		// I think we should add some additional fields for supporting all tags saving (TAG_TEXT_FIELD_7)
		if (m_useDeviceFont)
		{
			tag = TAG_TEXT_FIELD_2;
			swf->stream.writeBool(m_useDeviceFont);

			if (m_unknownFlag)
			{
				tag = TAG_TEXT_FIELD_3;
				swf->stream.finalizeTag(tag, pos);
				return;
			}

			if ((uint32_t)m_outlineColor > 0)
			{
				tag = TAG_TEXT_FIELD_4;
				swf->stream.writeInt(m_outlineColor);

				if (m_unknownShort != 0)
				{
					tag = TAG_TEXT_FIELD_5;

					swf->stream.writeShort(m_unknownShort);
					swf->stream.writeShort(0);

					if (m_bendAngle != 0)
					{
						tag = TAG_TEXT_FIELD_6;
						swf->stream.writeShort((int16_t)(m_bendAngle / 91.019f));

						if (m_autoAdjustFontSize)
						{
							tag = TAG_TEXT_FIELD_8;
							swf->stream.writeBool(m_autoAdjustFontSize);
						}
					}
				}
			}
		}

		swf->stream.finalizeTag(tag, pos);
	}
}