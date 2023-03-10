#include <filesystem>
#include <sys/stat.h>
#include <iostream>

#include "SupercellFlash/flash/SupercellSWF.h"

#include "SupercellFlash/error/UnableToFindObjectException.h"
#include "SupercellFlash/error/NegativeTagLengthException.h"
#include "SupercellFlash/error/UnsupportedTagException.h"
#include "SupercellFlash/error/DecompressionFailedException.h"

#include "SupercellFlash/flash/Matrix2x3.h"
#include "SupercellFlash/flash/ColorTransform.h"

#include "SupercellFlash/flash/Tag.h"

#include "SupercellFlash/compression/Decompressor.h"

namespace fs = std::filesystem;

namespace
{
	bool doesFileExist(const std::string& filePath)
	{

	}
}

namespace sc
{
	SupercellSWF::SupercellSWF()
	{
	}

	SupercellSWF::~SupercellSWF()
	{
	}

	DisplayObject& SupercellSWF::getDisplayObject(uint16_t id, const std::string& name)
	{
		for (uint16_t i = 0; i < m_shapes.size(); i++)
		{
			if (m_shapes[i].exportId() == id)
				return m_shapes[i];
		}
		
		// FIXME: rework it for getting object of any type by export name and export id.
		// Also solve problem with empty name arg (that means we don't want to search object by name, id only)
		// and empty export name field in object (that means it don't have export name)
		for (uint16_t i = 0; i < m_movieClips.size(); i++)
		{
			if ((m_movieClips[i].exportId() == id) && (name != "") && (m_movieClips[i].exportName() == name))
				return m_movieClips[i];
		}

		for (uint16_t i = 0; i < m_textFields.size(); i++)
		{
			if (m_textFields[i].exportId() == id)
				return m_textFields[i];
		}

		for (uint16_t i = 0; i < m_movieClipModifiers.size(); i++)
		{
			if (m_movieClipModifiers[i].exportId() == id)
				return m_movieClipModifiers[i];
		}

		std::string message = "Unable to find some DisplayObject id " + id;
		message += ", " + m_filePath;

		if (name != "")
			message += " needed by export name " + name;

		throw UnableToFindObjectException(message);
	}

	const std::vector<uint16_t>& SupercellSWF::exportIds()
	{
		std::vector<uint16_t> exportIds;

		for (uint16_t i = 0; i < m_shapes.size(); i++)
		{
			exportIds.push_back(m_shapes[i].exportId());
		}

		for (uint16_t i = 0; i < m_movieClips.size(); i++)
		{
			exportIds.push_back(m_movieClips[i].exportId());
		}

		for (uint16_t i = 0; i < m_textFields.size(); i++)
		{
			exportIds.push_back(m_textFields[i].exportId());
		}

		for (uint16_t i = 0; i < m_movieClipModifiers.size(); i++)
		{
			exportIds.push_back(m_movieClipModifiers[i].exportId());
		}

		return exportIds;
	}

	const std::vector<std::string>& SupercellSWF::exportNames()
	{
		std::vector<std::string> exportNames;

		for (uint16_t i = 0; i < m_movieClips.size(); i++)
		{
			exportNames.push_back(m_movieClips[i].exportName());
		}

		return exportNames;
	}

	void SupercellSWF::skip(int32_t length)
	{
		m_stream->skip(length);
	}

	bool SupercellSWF::readBool()
	{
		return m_stream->readBool();
	}

	int8_t SupercellSWF::readChar()
	{
		return m_stream->readChar();
	}

	uint8_t SupercellSWF::readUnsignedChar()
	{
		return m_stream->readUnsignedChar();
	}

	int16_t SupercellSWF::readShort()
	{
		return m_stream->readShort();
	}

	uint16_t SupercellSWF::readUnsignedShort()
	{
		return m_stream->readUnsignedShort();
	}

	int32_t SupercellSWF::readInt()
	{
		return m_stream->readInt();
	}

	float SupercellSWF::readTwip()
	{
		return m_stream->readTwip();
	}

	std::string SupercellSWF::readAscii()
	{
		return m_stream->readAscii();
	}

	std::string SupercellSWF::readFontName()
	{
		std::string fontName = readAscii();

		if (fontName != "")
		{
			if (!(std::find(m_fontNames.begin(), m_fontNames.end(), fontName) != m_fontNames.end()))
			{
				m_fontNames.push_back(fontName);
			}
		}

		return fontName;
	}

	std::vector<int8_t> SupercellSWF::readByteArray(uint32_t count)
	{
		return m_stream->readByteArray(count);
	}

	std::vector<int16_t> SupercellSWF::readShortArray(uint32_t count)
	{
		return m_stream->readShortArray(count);
	}

	std::vector<int32_t> SupercellSWF::readIntArray(uint32_t count)
	{
		return m_stream->readIntArray(count);
	}

	void SupercellSWF::load(const std::string& filePath)
	{
		m_filePath = filePath;

		loadInternal(filePath, false);

		if (m_useExternalTexture)
		{
			std::string textureFilePath;

			if (m_useUncommonResolutionTexture)
			{
				textureFilePath = m_uncommonResolutionTexturePath;
			}
			else
			{
				fs::path path(filePath);
				path = fs::path(path.root_path()).concat(path.stem().string()).concat("_tex.sc");

				textureFilePath = path.string();
			}

			loadInternal(textureFilePath, true);
		}
	}

	void SupercellSWF::save(const std::string& filePath)
	{
	}

	void SupercellSWF::loadInternal(const std::string& filePath, bool isTextureFile)
	{
		// FILE* compressedFile = fopen(filePath.c_str(), "rb");
		// ByteStream compressedStream(compressedFile);

		FILE* decompressedFile = fopen(filePath.c_str(), "rb");
		// ByteStream decompressedStream(decompressedFile);

		m_stream = new ByteStream(decompressedFile);

		// Decompressor::decompress(compressedStream, *m_stream);

		if (isTextureFile)
		{
			loadTags();
			return;
		}

		m_shapesCount = readUnsignedShort();
		for (uint16_t i = 0; i < m_shapesCount; i++)
			m_shapes.push_back(Shape());

		m_movieClipModifiersCount = readUnsignedShort();
		for (uint16_t i = 0; i < m_movieClipsCount; i++)
			m_movieClips.push_back(MovieClip());

		m_texturesCount = readUnsignedShort();
		for (uint16_t i = 0; i < m_texturesCount; i++)
			m_textures.push_back(SWFTexture());

		m_textFieldsCount = readUnsignedShort();
		for (uint16_t i = 0; i < m_textFieldsCount; i++)
			m_textFields.push_back(TextField());

		uint16_t matricesCount = readUnsignedShort();
		uint16_t colorTransformsCount = readUnsignedShort();

		MatrixBank matrixBank;

		for (uint16_t i = 0; i < matricesCount; i++)
			matrixBank.matrices.push_back(Matrix2x3());

		for (uint16_t i = 0; i < colorTransformsCount; i++)
			matrixBank.colorTransforms.push_back(ColorTransform());

		m_matrixBanks.push_back(matrixBank);

		skip(5);

		uint16_t exportsCount = readUnsignedShort();

		std::vector<uint16_t> exportIds;
		std::vector<std::string> exportNames;

		for (uint16_t i = 0; i < exportsCount; i++)
		{
			exportIds.push_back(readUnsignedShort());
			std::cout << exportIds[i] << std::endl;
		}

		for (uint16_t i = 0; i < exportsCount; i++)
		{
			exportNames.push_back(readAscii());
			std::cout << exportNames[i] << std::endl;
		}

		loadTags();

		for (uint16_t i = 0; i < exportsCount; i++)
		{
			getDisplayObject(exportIds[i], "").exportName(exportNames[i]);
			std::cout << "Assigned " << exportNames[i] << " name to DisplayObject with id " << exportIds[i] << std::endl;
		}
	}

	void SupercellSWF::loadTags()
	{
		uint16_t loadedShapes = 0;
		uint16_t loadedMovieClips = 0;
		uint16_t loadedTextures = 0;
		uint16_t loadedTextFields = 0;
		uint16_t loadedMatrices = 0;
		uint16_t loadedColorTransforms = 0;
		uint16_t loadedMovieClipModifiers = 0;
		uint8_t loadedMatrixBanks = 0;

		while (true)
		{
			uint8_t tag = readUnsignedChar();
			int32_t tagLength = readInt();

			std::cout << "Tag " << (int)tag << ", length " << tagLength << std::endl;

			if (tagLength < 0)
			{
				std::string message = "Negative tag length. Tag " + tag;
				throw NegativeTagLengthException(message + ", " + m_filePath);
			}

			switch (tag)
			{
			case TAG_END:
				return;

			case TAG_USE_HALF_SCALE_TEXTURE:
				m_useHalfScaleTexture = true;
				break;

			case TAG_USE_EXTERNAL_TEXTURE:
				m_useExternalTexture = true;
				break;

			case TAG_USE_UNCOMMON_RESOLUTION_TEXTURE:
				{
					m_useUncommonResolutionTexture = true;

					fs::path withoutExtension = fs::path(m_filePath).stem();

					fs::path uncommonFilePath = withoutExtension.concat(m_UncommonResolutionFileSuffix).concat("_tex.sc");
					fs::path halfScaleFilePath = withoutExtension.concat(m_halfScaleFileSuffix).concat("_tex.sc");

					m_useHalfScaleTexture= true;
					m_uncommonResolutionTexturePath = uncommonFilePath.string();

					if (!fs::exists(uncommonFilePath))
					{
						if (fs::exists(halfScaleFilePath))
						{
							m_uncommonResolutionTexturePath = halfScaleFilePath.string();
						}
					}
				}
				break;

			case TAG_TEXTURE:
			case TAG_TEXTURE_2:
			case TAG_TEXTURE_3:
			case TAG_TEXTURE_4:
			case TAG_TEXTURE_5:
			case TAG_TEXTURE_6:
			case TAG_TEXTURE_7:
			case TAG_TEXTURE_8:
				loadedTextures++;
				break;

			case TAG_MOVIE_CLIP_MODIFIERS_COUNT:
				m_movieClipModifiersCount = readUnsignedShort();
				for (uint16_t i = 0; i < m_movieClipModifiersCount; i++)
					m_movieClipModifiers.push_back(MovieClipModifier());

				break;

			case TAG_MOVIE_CLIP_MODIFIER_MASK:
			case TAG_MOVIE_CLIP_MODIFIER_MASKED:
			case TAG_MOVIE_CLIP_MODIFIER_UNMASKED:
				{
					m_movieClipModifiers[loadedMovieClipModifiers].load(this, tag);
					loadedMovieClipModifiers++;
				}
				break;

			case TAG_SHAPE:
			case TAG_SHAPE_2:
				{
					m_shapes[loadedShapes].load(this, tag);
					loadedShapes++;
				}
				break;

			case TAG_TEXT_FIELD:
			case TAG_TEXT_FIELD_2:
			case TAG_TEXT_FIELD_3:
			case TAG_TEXT_FIELD_4:
			case TAG_TEXT_FIELD_5:
			case TAG_TEXT_FIELD_6:
			case TAG_TEXT_FIELD_7:
			case TAG_TEXT_FIELD_8:
				{
					m_textFields[loadedTextFields].load(this, tag);
					loadedTextFields++;
				}
				break;

			case TAG_MATRIX_BANK:
				{

					uint16_t matricesCount = readUnsignedShort();
					uint16_t colorTransformsCount = readUnsignedShort();

					MatrixBank matrixBank;

					for (uint16_t i = 0; i < matricesCount; i++)
						matrixBank.matrices.push_back(Matrix2x3());

					for (uint16_t i = 0; i < colorTransformsCount; i++)
						matrixBank.colorTransforms.push_back(ColorTransform());

					m_matrixBanks.push_back(matrixBank);

					loadedMatrices = 0;
					loadedColorTransforms = 0;

					loadedMatrixBanks++;
				}
				break;

			case TAG_MATRIX:
			case TAG_MATRIX_PRECISE:
				{
					m_matrixBanks[loadedMatrixBanks].matrices[loadedMatrices].load(this, tag);
					loadedMatrices++;
				}
				break;

			case TAG_COLOR_TRANSFORM:
				{
					m_matrixBanks[loadedMatrixBanks].colorTransforms[loadedColorTransforms].load(this);
					loadedColorTransforms++;
				}
				break;

			case TAG_TIMELINE_INDEXES:
				{
					int32_t indexesCount = readInt();
					for (int32_t i = 0; i < indexesCount; i++)
						readUnsignedShort();

					throw UnsupportedTagException("TAG_TIMELINE_INDEXES no longer in use");
				}
				break;

			case TAG_MOVIE_CLIP:
			case TAG_MOVIE_CLIP_2:
			case TAG_MOVIE_CLIP_3:
			case TAG_MOVIE_CLIP_4:
			case TAG_MOVIE_CLIP_5:
				{
					m_movieClips[loadedMovieClips].load(this, tag);
					loadedMovieClips++;
				}
				break;

			default:
				{
					std::string message = "Encountered unknown tag " + tag;
					throw UnsupportedTagException(message + ", " + m_filePath);
				}
				break;
			}
		}
	}

	void SupercellSWF::saveInternal(const std::string& filePath, bool isTextureFile)
	{
	}

	void SupercellSWF::saveTags(bool isTextureFile)
	{
	}

}
