#include "SupercellFlash/flash/objects/TextField.h"

#include "SupercellFlash/flash/SupercellSWF.h"

namespace sc
{
	void TextField::load(SupercellSWF* swf, uint8_t tag)
	{
		m_exportId = swf->readUnsignedShort();

		m_fontName = swf->readFontName();
		m_fontColor = swf->readInt();

		m_isBold = swf->readBool();
		m_isItalic = swf->readBool();
		m_isMultiline = swf->readBool();
		swf->readBool();

		m_align = swf->readUnsignedChar();
		m_fontSize = swf->readUnsignedChar();

		m_left = swf->readShort();
		m_top = swf->readShort();
		m_right = swf->readShort();
		m_bottom = swf->readShort();

		m_isOutlineEnabled = swf->readBool();

		m_text = swf->readAscii();

		if (tag == TAG_TEXT_FIELD)
			return;

		m_usingDeviceFont = swf->readBool();

		switch (tag)
		{
		case TAG_TEXT_FIELD_2:
			return;

		case TAG_TEXT_FIELD_3:
			m_unknownFlag = true;
			return;

		case TAG_TEXT_FIELD_4:
			m_unknownFlag = true;
			m_outlineColor = swf->readInt();
			return;

		case TAG_TEXT_FIELD_5:
			m_outlineColor = swf->readInt();
			return;

		case TAG_TEXT_FIELD_6:
		case TAG_TEXT_FIELD_7:
		case TAG_TEXT_FIELD_8:
			{
				m_outlineColor = swf->readInt();
				m_unknownShort = swf->readShort();
				swf->readShort();

				m_unknownFlag = true;

				if (tag == TAG_TEXT_FIELD_6)
					return;

				m_bendAngle = (float)swf->readShort() * 91.019f;

				if (tag == TAG_TEXT_FIELD_7)
					return;

				m_autoAdjustFontSize = swf->readBool();
				return;
			}
			return;
		}
	}
}
