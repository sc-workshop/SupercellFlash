#include "SupercellFlash/SupercellSWF.h"
#include "error/NegativeTagLengthException.h"
#include "SupercellBytestream/error/StreamException.h"

#include <filesystem>

#include <iostream>

namespace sc
{
	SupercellSWF::SupercellSWF()
	{
	}

	SupercellSWF::~SupercellSWF()
	{
	}

	void SupercellSWF::load(const std::string& filePath)
	{
		m_useExternalTexture = loadInternal(filePath, false);

		if (m_useExternalTexture)
		{
			std::filesystem::path path(filePath);

			std::filesystem::path multiResFilePath = std::filesystem::path(path.root_path()).concat(path.stem().string()).concat(m_multiResFileSuffix + "_tex.sc");
			std::filesystem::path lowResFilePath = std::filesystem::path(path.root_path()).concat(path.stem().string()).concat(m_lowResFileSuffix + "_tex.sc");
			std::filesystem::path externalFilePath = std::filesystem::path(path.root_path()).concat(path.stem().string()).concat("_tex.sc");

			if (m_useMultiResTexture && std::filesystem::exists(multiResFilePath))
			{
				loadInternal(multiResFilePath.string(), true);
			}
			else if (m_useLowResTexture && std::filesystem::exists(lowResFilePath))
			{
				loadInternal(lowResFilePath.string(), true);
			}
			else if (std::filesystem::exists(externalFilePath))
			{
				loadInternal(externalFilePath.string(), true);
			}
			else
			{
				throw StreamException(StreamError::EXIST_ERROR, "Cannot find external *_tex.sc file");
			}
		}
	}

	void SupercellSWF::save(const std::string& filepath, CompressionSignature signature)
	{
		stream.clear();
		saveInternal();
		stream.save(filepath, signature);

		if (m_useExternalTexture) {
			std::filesystem::path path(filepath);
			std::filesystem::path multiResFilePath = std::filesystem::path(path.root_path()).concat(path.stem().string()).concat(m_multiResFileSuffix + "_tex.sc");
			std::filesystem::path lowResFilePath = std::filesystem::path(path.root_path()).concat(path.stem().string()).concat(m_lowResFileSuffix + "_tex.sc");
			std::filesystem::path externalFilePath = std::filesystem::path(path.root_path()).concat(path.stem().string()).concat("_tex.sc");

			for (SWFTexture texture : textures) {
				if (texture.data.size() == 0) {
					stream.clear();
					return;
				}
				texture.save(this, true, false);
			}
			stream.writeTag(0);

			stream.save(m_useMultiResTexture ? multiResFilePath.string() : externalFilePath.string(), signature);

			if (m_useLowResTexture || m_useMultiResTexture) {
				for (SWFTexture texture : textures) {
					texture.save(this, true, true);
				}
				stream.writeTag(0);

				stream.save(lowResFilePath.string(), signature);
			}
		}
	}

	bool SupercellSWF::loadInternal(std::string filepath, bool isTexture)
	{
		stream.open(filepath);

		// Reading .sc file
		if (!isTexture)
		{
			uint16_t shapesCount = stream.readUnsignedShort();
			shapes = std::vector<Shape>(shapesCount);

			uint16_t movieClipsCount = stream.readUnsignedShort();
			movieClips = std::vector<MovieClip>(movieClipsCount);

			uint16_t texturesCount = stream.readUnsignedShort();
			textures = std::vector<SWFTexture>(texturesCount);

			uint16_t textFieldsCount = stream.readUnsignedShort();
			textFields = std::vector<TextField>(textFieldsCount);

			uint16_t matricesCount = stream.readUnsignedShort();
			uint16_t colorTransformsCount = stream.readUnsignedShort();
			initMatrixBank(matricesCount, colorTransformsCount);

			stream.skip(5); // unused

			uint16_t exportsCount = stream.readUnsignedShort();
			exports = std::vector < ExportName > (exportsCount);

			for (uint16_t i = 0; i < exportsCount; i++)
			{
				exports[i].id = stream.readUnsignedShort();
			}

			for (uint16_t i = 0; i < exportsCount; i++)
			{
				exports[i].name = stream.readAscii();
			}
		}

		bool isHasTextureLoadTag = loadTags();
		stream.clear();
		return isHasTextureLoadTag;
	}

	bool SupercellSWF::loadTags()
	{
		bool useExternalTexture = false;

		uint16_t shapesLoaded = 0;
		uint16_t movieClipsLoaded = 0;
		uint16_t texturesLoaded = 0;
		uint16_t textFieldsLoaded = 0;
		uint8_t matrixBanksLoaded = 0;
		uint16_t matricesLoaded = 0;
		uint16_t colorTransformsLoaded = 0;
		uint16_t movieClipModifiersLoaded = 0;

		while (true)
		{
			uint8_t tag = stream.readUnsignedByte();
			int32_t tagLength = stream.readInt();

			if (tag == TAG_END)
				break;

			if (tagLength < 0)
				throw NegativeTagLengthException(tag);

			switch (tag)
			{
			case TAG_USE_LOW_RES_TEXTURE:
				m_useLowResTexture = true;
				break;

			case TAG_USE_EXTERNAL_TEXTURE:
				useExternalTexture = true;
				break;

			case TAG_USE_MULTI_RES_TEXTURE:
				m_useMultiResTexture = true;
				break;

			case TAG_TEXTURE_FILE_SUFFIXES:
				m_multiResFileSuffix = stream.readAscii();
				m_lowResFileSuffix = stream.readAscii();
				break;

			case TAG_TEXTURE:
			case TAG_TEXTURE_2:
			case TAG_TEXTURE_3:
			case TAG_TEXTURE_4:
			case TAG_TEXTURE_5:
			case TAG_TEXTURE_6:
			case TAG_TEXTURE_7:
			case TAG_TEXTURE_8:
				if (textures.size() < texturesLoaded) {
					throw std::runtime_error("Trying to load too many textures");
				}
				textures[texturesLoaded].load(this, tag, useExternalTexture);
				texturesLoaded++;
				break;

			case TAG_MOVIE_CLIP_MODIFIERS_COUNT: {
				uint16_t movieClipModifiersCount = stream.readUnsignedShort();
				movieClipModifiers = std::vector<MovieClipModifier>(movieClipModifiersCount);
				break;
			}

			case TAG_MOVIE_CLIP_MODIFIER:
			case TAG_MOVIE_CLIP_MODIFIER_2:
			case TAG_MOVIE_CLIP_MODIFIER_3:
				movieClipModifiers[movieClipModifiersLoaded].load(this, tag);
				movieClipModifiersLoaded++;
				break;

			case TAG_SHAPE:
			case TAG_SHAPE_2:
				if (shapes.size() < shapesLoaded) {
					throw std::runtime_error("Trying to load too many Shapes");
				}
				shapes[shapesLoaded].load(this, tag);
				shapesLoaded++;
				break;

			case TAG_TEXT_FIELD:
			case TAG_TEXT_FIELD_2:
			case TAG_TEXT_FIELD_3:
			case TAG_TEXT_FIELD_4:
			case TAG_TEXT_FIELD_5:
			case TAG_TEXT_FIELD_6:
			case TAG_TEXT_FIELD_7:
			case TAG_TEXT_FIELD_8:
				if (textFields.size() < textFieldsLoaded) {
					throw std::runtime_error("Trying to load too many TextFields");
				}

				textFields[textFieldsLoaded].load(this, tag);
				textFieldsLoaded++;
				break;

			case TAG_MATRIX_BANK:
				matricesLoaded = 0;
				colorTransformsLoaded = 0;
				matrixBanksLoaded++;
				{
					uint16_t matrixCount = stream.readUnsignedShort();
					uint16_t colorTransformCount = stream.readUnsignedShort();
					initMatrixBank(matrixCount, colorTransformCount);
				}
				break;

			case TAG_MATRIX_2x3:
			case TAG_MATRIX_2x3_2:
				matrixBanks[matrixBanksLoaded].matrices[matricesLoaded].load(this, tag);
				matricesLoaded++;
				break;

			case TAG_COLOR_TRANSFORM:
				matrixBanks[matrixBanksLoaded].colorTransforms[colorTransformsLoaded].load(this);
				colorTransformsLoaded++;
				break;

			case TAG_MOVIE_CLIP:
			case TAG_MOVIE_CLIP_2:
			case TAG_MOVIE_CLIP_3:
			case TAG_MOVIE_CLIP_4:
			case TAG_MOVIE_CLIP_5:
				if (movieClips.size() < movieClipsLoaded) {
					throw std::runtime_error("Trying to load too many MovieClips");
				}
				movieClips[movieClipsLoaded].load(this, tag);
				movieClipsLoaded++;
				break;

			default:
				stream.skip(tagLength);
				break;
			}
		}

		return useExternalTexture;
	}

	void SupercellSWF::saveInternal()
	{
		if (matrixBanks.size() == 0)
			matrixBanks = std::vector<MatrixBank>(0);

		uint16_t exportsCount = static_cast<uint16_t>(exports.size());
		uint16_t shapeCount = static_cast<uint16_t>(shapes.size());
		uint16_t movieClipsCount = static_cast<uint16_t>(movieClips.size());
		uint16_t texturesCount = static_cast<uint16_t>(textures.size());
		uint16_t textFieldsCount = static_cast<uint16_t>(textFields.size());

		stream.writeUnsignedShort(shapeCount);
		stream.writeUnsignedShort(movieClipsCount);
		stream.writeUnsignedShort(texturesCount);
		stream.writeUnsignedShort(textFieldsCount);

		stream.writeUnsignedShort(static_cast<uint16_t>(matrixBanks[0].matrices.size()));
		stream.writeUnsignedShort(static_cast<uint16_t>(matrixBanks[0].colorTransforms.size()));

		// unused 5 bytes
		stream.writeUnsignedByte(0);
		stream.writeInt(0);

		stream.writeUnsignedShort(exportsCount);

		for (uint16_t i = 0; exportsCount > i; i++)
		{
			stream.writeUnsignedShort(exports[i].id);
		}

		for (uint16_t i = 0; exportsCount > i; i++)
		{
			stream.writeAscii(exports[i].name);
		}

		saveTags(shapeCount, movieClipsCount, texturesCount, textFieldsCount);
	}

	void SupercellSWF::saveTags(
		uint16_t shapeCount,
		uint16_t movieClipsCount,
		uint16_t texturesCount,
		uint16_t textFieldsCount
	)
	{
		if (m_useLowResTexture)
			stream.writeTag(TAG_USE_LOW_RES_TEXTURE);

		if (m_useExternalTexture)
			stream.writeTag(TAG_USE_EXTERNAL_TEXTURE);

		if (m_useMultiResTexture)
			stream.writeTag(TAG_USE_MULTI_RES_TEXTURE);

		for (SWFTexture texture : textures) {
			texture.save(this, !m_useExternalTexture, false);
		}

		if (movieClipModifiers.size() > 0) {
			uint16_t modifiersCount = static_cast<uint16_t>(movieClipModifiers.size());
			stream.writeUnsignedByte(TAG_MOVIE_CLIP_MODIFIERS_COUNT);
			stream.writeInt(sizeof(uint16_t));
			stream.writeUnsignedShort(modifiersCount);
		}

		for (uint16_t i = 0; shapeCount > i; i++) {
			shapes[i].save(this);
		}

		for (uint16_t i = 0; textFieldsCount > i; i++) {
			textFields[i].save(this);
		}

		uint8_t matrixBanksCount = static_cast<uint8_t>(matrixBanks.size());

		for (uint8_t i = 0; matrixBanksCount > i; i++) {
			uint16_t matricesCount = static_cast<uint16_t>(matrixBanks[i].matrices.size());
			uint16_t colorsCount = static_cast<uint16_t>(matrixBanks[i].colorTransforms.size());

			for (uint16_t m = 0; matricesCount > m; m++) {
				sc::Matrix2x3 matrix = matrixBanks[i].matrices[m];
				matrixBanks[i].matrices[m].save(this);
			}

			for (uint16_t c = 0; colorsCount > c; c++) {
				matrixBanks[i].colorTransforms[c].save(this);
			}
		}

		for (uint16_t i = 0; movieClipsCount > i; i++) {
			movieClips[i].save(this);
		}

		stream.writeTag(TAG_END); // EoF
	}

	void SupercellSWF::initMatrixBank(uint16_t matricesCount, uint16_t colorTransformsCount)
	{
		MatrixBank bank;
		bank.matrices = std::vector<Matrix2x3>(matricesCount);
		bank.colorTransforms = std::vector<ColorTransform>(colorTransformsCount);
		matrixBanks.push_back(bank);
	}
}
