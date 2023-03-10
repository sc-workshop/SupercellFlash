#include "SupercellFlash/flash/SWFTexture.h"

#include "SupercellFlash/flash/SupercellSWF.h"

namespace sc
{
	void SWFTexture::load(SupercellSWF* swf, uint8_t tag, bool hasTexture)
	{
		uint8_t pixelTypeIndex = swf->readUnsignedChar();

		uint8_t pixelSizeInBytes;

		switch (pixelTypeIndex)
		{
		case 2:
		case 9:
			m_pixelType = PixelType::RGBA4;
			pixelSizeInBytes = 2;
			break;

		case 3:
			m_pixelType = PixelType::RGB5_A1;
			pixelSizeInBytes = 2;
			break;

		case 4:
			m_pixelType = PixelType::RGB565;
			pixelSizeInBytes = 2;
			break;

		case 6:
			m_pixelType = PixelType::LUMINANCE8_ALPHA8;
			pixelSizeInBytes = 2;
			break;

		case 10:
			m_pixelType = PixelType::LUMINANCE8;
			pixelSizeInBytes = 1;
			break;

		default:
			m_pixelType = PixelType::RGBA8;
			pixelSizeInBytes = 4;
			break;
		}

		m_filter = TextureFilter::LinearNearest;

		if (tag == TAG_TEXTURE_2 || tag == TAG_TEXTURE_3 || tag == TAG_TEXTURE_7)
		{
			m_filter = TextureFilter::LinearMipmapNearest;
		}
		else if (tag == TAG_TEXTURE_8)
		{
			m_filter = TextureFilter::NearestNearest;
		}

		m_interlace = tag == TAG_TEXTURE_5 || tag == TAG_TEXTURE_6 || tag == TAG_TEXTURE_7;
		m_mipmaps = tag == TAG_TEXTURE || tag == TAG_TEXTURE_2 || tag == TAG_TEXTURE_6 || tag == TAG_TEXTURE_7;

		m_width = swf->readUnsignedShort();
		m_height = swf->readUnsignedShort();

		if (!hasTexture)
			return;

		
	}
}
