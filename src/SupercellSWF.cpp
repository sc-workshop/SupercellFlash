#include "SupercellFlash/SupercellSWF.h"
#include "SupercellFlash/error/NegativeTagLengthException.h"
#include "SupercellFlash/error/ObjectLoadingException.h"
#include "SupercellFlash/error/NullPointerException.h"

namespace sc
{
	void SupercellSWF::load(const fs::path& path)
	{
		m_useExternalTexture = loadInternal(path, false);

		if (m_useExternalTexture)
		{
			fs::path path(path);
			fs::path basename = path.stem();
			path.remove_filename();

			fs::path multiResFilePath = path / fs::path(basename).concat(m_multiResFileSuffix + "_tex.sc");
			fs::path lowResFilePath = path / fs::path(basename).concat(m_lowResFileSuffix + "_tex.sc");
			fs::path externalFilePath = path / fs::path(basename).concat(path.stem().string()).concat("_tex.sc");

			if (m_useMultiResTexture && filesystem::exists(multiResFilePath))
			{
				loadInternal(multiResFilePath.string(), true);
			}
			else if (m_useLowResTexture && filesystem::exists(lowResFilePath))
			{
				loadInternal(lowResFilePath.string(), true);
			}
			else if (filesystem::exists(externalFilePath))
			{
				loadInternal(externalFilePath.string(), true);
			}
			else
			{
				throw std::exception("Cannot find external *_tex.sc file");
			}
		}
	}

	void SupercellSWF::save(const fs::path& filepath, CompressionSignature signature)
	{
		saveInternal();
		stream.save(filepath, signature);

		if (m_useExternalTexture) {
			fs::path path(filepath);
			fs::path basename = path.stem();
			path.remove_filename();

			fs::path multiResFilePath = path / fs::path(basename).concat(m_multiResFileSuffix + "_tex.sc");
			fs::path lowResFilePath = path / fs::path(basename).concat(m_lowResFileSuffix + "_tex.sc");
			fs::path externalFilePath = path / fs::path(basename).concat(path.stem().string()).concat("_tex.sc");

			stream.init();
			for (pSWFTexture texture : textures) {
				texture->save(this, true, false);
			}
			stream.writeTag(0);
			stream.save(m_useMultiResTexture ? multiResFilePath.string() : externalFilePath.string(), signature);

			if (m_useLowResTexture || m_useMultiResTexture) {
				stream.init();
				for (pSWFTexture texture : textures) {
					texture->save(this, true, true);
				}
				stream.writeTag(0);
				stream.save(lowResFilePath.string(), signature);
			}
		}
	}

	bool SupercellSWF::loadInternal(const fs::path& filepath, bool isTexture)
	{
		stream.open(filepath);

		// Reading .sc file
		if (!isTexture)
		{
			uint16_t shapesCount = stream.readUnsignedShort();
			shapes = vector<pShape>(shapesCount);

			uint16_t movieClipsCount = stream.readUnsignedShort();
			movieClips = vector<pMovieClip>(movieClipsCount);

			uint16_t texturesCount = stream.readUnsignedShort();
			textures = vector<pSWFTexture>(texturesCount);

			uint16_t textFieldsCount = stream.readUnsignedShort();
			textFields = vector<pTextField>(textFieldsCount);

			uint16_t matricesCount = stream.readUnsignedShort();
			uint16_t colorTransformsCount = stream.readUnsignedShort();
			initMatrixBank(matricesCount, colorTransformsCount);

			stream.skip(5); // unused

			uint16_t exportsCount = stream.readUnsignedShort();
			exports = vector <pExportName>(exportsCount);

			for (uint16_t i = 0; i < exportsCount; i++)
			{
				exports[i] = pExportName(new ExportName());

				exports[i]->id(stream.readUnsignedShort());
			}

			for (uint16_t i = 0; i < exportsCount; i++)
			{
				exports[i]->name(stream.readAscii());
			}
		}

		bool isHasTextureLoadTag = loadTags();
		stream.close();
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
			case TAG_USE_MULTI_RES_TEXTURE:
				m_useMultiResTexture = true;
				break;

			case TAG_DISABLE_LOW_RES_TEXTURE:
				m_useLowResTexture = false;
				break;

			case TAG_USE_EXTERNAL_TEXTURE:
				useExternalTexture = true;
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
			case TAG_TEXTURE_9:
				if (textures.size() < texturesLoaded) {
					throw ObjectLoadingException("Trying to load too many textures");
				}

				textures[texturesLoaded] = pSWFTexture((new SWFTexture())->load(this, tag, useExternalTexture));
				texturesLoaded++;
				break;

			case TAG_MOVIE_CLIP_MODIFIERS_COUNT: {
				uint16_t movieClipModifiersCount = stream.readUnsignedShort();
				movieClipModifiers = vector<pMovieClipModifier>(movieClipModifiersCount);
				break;
			}

			case TAG_MOVIE_CLIP_MODIFIER:
			case TAG_MOVIE_CLIP_MODIFIER_2:
			case TAG_MOVIE_CLIP_MODIFIER_3:
				movieClipModifiers[movieClipModifiersLoaded] = pMovieClipModifier((new MovieClipModifier())->load(this, tag));
				movieClipModifiersLoaded++;
				break;

			case TAG_SHAPE:
			case TAG_SHAPE_2:
				if (shapes.size() < shapesLoaded) {
					throw ObjectLoadingException("Trying to load too many Shapes");
				}
				shapes[shapesLoaded] = pShape((new Shape())->load(this, tag));
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
					throw ObjectLoadingException("Trying to load too many TextFields");
				}

				textFields[textFieldsLoaded] = pTextField((new TextField())->load(this, tag));
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
				matrixBanks[matrixBanksLoaded]->matrices[matricesLoaded] = pMatrix2D((new Matrix2D())->load(this, tag));
				matricesLoaded++;
				break;

			case TAG_COLOR_TRANSFORM:
				matrixBanks[matrixBanksLoaded]->colorTransforms[colorTransformsLoaded] = pColorTransform((new ColorTransform())->load(this));
				colorTransformsLoaded++;
				break;

			case TAG_MOVIE_CLIP:
			case TAG_MOVIE_CLIP_2:
			case TAG_MOVIE_CLIP_3:
			case TAG_MOVIE_CLIP_4:
			case TAG_MOVIE_CLIP_5:
				if (movieClips.size() < movieClipsLoaded) {
					throw ObjectLoadingException("Trying to load too many MovieClips");
				}
				movieClips[movieClipsLoaded] = pMovieClip((new MovieClip())->load(this, tag));
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
		stream.init();

		if (matrixBanks.size() == 0) {
			matrixBanks.push_back(pMatrixBank(new MatrixBank()));
		}

		uint16_t exportsCount = static_cast<uint16_t>(exports.size());
		uint16_t shapeCount = static_cast<uint16_t>(shapes.size());
		uint16_t movieClipsCount = static_cast<uint16_t>(movieClips.size());
		uint16_t texturesCount = static_cast<uint16_t>(textures.size());
		uint16_t textFieldsCount = static_cast<uint16_t>(textFields.size());

		stream.writeUnsignedShort(shapeCount);
		stream.writeUnsignedShort(movieClipsCount);
		stream.writeUnsignedShort(texturesCount);
		stream.writeUnsignedShort(textFieldsCount);

		stream.writeUnsignedShort(static_cast<uint16_t>(matrixBanks[0]->matrices.size()));
		stream.writeUnsignedShort(static_cast<uint16_t>(matrixBanks[0]->colorTransforms.size()));

		// unused 5 bytes
		stream.writeUnsignedByte(0);
		stream.writeInt(0);

		stream.writeUnsignedShort(exportsCount);

		for (uint16_t i = 0; exportsCount > i; i++)
		{
			stream.writeUnsignedShort(exports[i]->id());
		}

		for (uint16_t i = 0; exportsCount > i; i++)
		{
			stream.writeAscii(exports[i]->name());
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
		if (m_useExternalTexture) {
			if (m_multiResFileSuffix != MULTIRES_DEFAULT_SUFFIX ||
				m_lowResFileSuffix != LOWRES_DEFAULT_SUFFIX) {
				uint32_t pos = stream.initTag();
				stream.writeAscii(m_multiResFileSuffix);
				stream.writeAscii(m_lowResFileSuffix);
				stream.finalizeTag(TAG_TEXTURE_FILE_SUFFIXES, pos);
			}

			if (!m_useLowResTexture)
				stream.writeTag(TAG_DISABLE_LOW_RES_TEXTURE);

			if (m_useMultiResTexture)
				stream.writeTag(TAG_USE_MULTI_RES_TEXTURE);

			stream.writeTag(TAG_USE_EXTERNAL_TEXTURE);
		}

		for (pSWFTexture texture : textures) {
			if (texture == nullptr) {
				throw NullPointerException<SWFTexture>();
			}
			texture->save(this, !m_useExternalTexture, false);
		}

		if (movieClipModifiers.size() > 0) {
			uint16_t modifiersCount = static_cast<uint16_t>(movieClipModifiers.size());
			stream.writeUnsignedByte(TAG_MOVIE_CLIP_MODIFIERS_COUNT);
			stream.writeInt(sizeof(uint16_t));
			stream.writeUnsignedShort(modifiersCount);

			for (pMovieClipModifier modifier : movieClipModifiers) {
				if (modifier == nullptr) {
					throw NullPointerException<MovieClipModifier>();
				}

				modifier->save(this);
			}
		}

		for (uint16_t i = 0; shapeCount > i; i++) {
			shapes[i]->save(this);
		}

		for (uint16_t i = 0; textFieldsCount > i; i++) {
			textFields[i]->save(this);
		}

		uint8_t matrixBanksCount = static_cast<uint8_t>(matrixBanks.size());

		for (uint8_t i = 0; matrixBanksCount > i; i++) {
			uint16_t matricesCount = static_cast<uint16_t>(matrixBanks[i]->matrices.size());
			uint16_t colorsCount = static_cast<uint16_t>(matrixBanks[i]->colorTransforms.size());

			if (i != 0) {
				uint32_t bankPos = stream.initTag();
				stream.writeUnsignedShort(matricesCount);
				stream.writeUnsignedShort(colorsCount);
				stream.finalizeTag(TAG_MATRIX_BANK, bankPos);
			}

			for (uint16_t m = 0; matricesCount > m; m++) {
				matrixBanks[i]->matrices[m]->save(this, m_usePrecisionMatrices);
			}

			for (uint16_t c = 0; colorsCount > c; c++) {
				matrixBanks[i]->colorTransforms[c]->save(this);
			}
		}

		for (uint16_t i = 0; movieClipsCount > i; i++) {
			movieClips[i]->save(this);
		}

		stream.writeTag(TAG_END); // EoF
	}

	void SupercellSWF::initMatrixBank(uint16_t matricesCount, uint16_t colorTransformsCount)
	{
		MatrixBank* bank = new MatrixBank();
		bank->matrices = vector<pMatrix2D>(matricesCount);
		bank->colorTransforms = vector<pColorTransform>(colorTransformsCount);

		matrixBanks.push_back(pMatrixBank(bank));
	}
}