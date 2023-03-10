#pragma once

#include <string>
#include <vector>

#include "SupercellFlash/io/ByteStream.h"

#include "SupercellFlash/flash/objects/Shape.h"
#include "SupercellFlash/flash/objects/MovieClip.h"
#include "SupercellFlash/flash/SWFTexture.h"
#include "SupercellFlash/flash/objects/TextField.h"
#include "SupercellFlash/flash/objects/MovieClipModifier.h"

#include "SupercellFlash/flash/MatrixBank.h"

#include "SupercellFlash/flash/Tag.h"

namespace sc
{
	class SupercellSWF
	{
	public:
		SupercellSWF();
		virtual ~SupercellSWF();

	public:
		std::string filePath() const { return m_filePath; }

		bool useExternalTexture() const { return m_useExternalTexture; }
		void useExternalTexture(bool status) { m_useExternalTexture = status; }

		bool useUncommonResolutionTexture() const { return m_useUncommonResolutionTexture; }
		void useUncommonResolutionTexture(bool status) { m_useUncommonResolutionTexture = status; }

		bool useHalfScaleTexture() const { return m_useHalfScaleTexture; }
		void useHalfScaleTexture(bool status) { m_useHalfScaleTexture = status; }

		DisplayObject& getDisplayObject(uint16_t id, const std::string& name);

		std::vector<Shape>& shapes() { return m_shapes; }
		void shapes(const std::vector<Shape>& shapes) { m_shapes = shapes; }

		std::vector<MovieClip>& movieClips() { return m_movieClips; }
		void movieClips(const std::vector<MovieClip>& movieClips) { m_movieClips = movieClips; }

		std::vector<SWFTexture>& textures() { return m_textures; }
		void textures(const std::vector<SWFTexture>& textures) { m_textures = textures; }

		std::vector<TextField>& textFields() { return m_textFields; }
		void textFields(const std::vector<TextField>& textFields) { m_textFields = textFields; }

		std::vector<MovieClipModifier>& movieClipModifiers() { return m_movieClipModifiers; }
		void movieClipModifiers(const std::vector<MovieClipModifier>& movieClipModifiers) { m_movieClipModifiers = movieClipModifiers; }

		std::vector<std::string>& fontNames() { return m_fontNames; }
		void fontNames(const std::vector<std::string>& fontNames) { m_fontNames = fontNames; }

		std::vector<MatrixBank>& matrixBanks() { return m_matrixBanks; }
		void matrixBanks(const std::vector<MatrixBank>& matrixBanks) { m_matrixBanks = matrixBanks; }

		const std::vector<uint16_t>& exportIds();
		const std::vector<std::string>& exportNames();

	public:
		void skip(int32_t length);

		bool readBool();

		int8_t readChar();
		uint8_t readUnsignedChar();

		int16_t readShort();
		uint16_t readUnsignedShort();

		int32_t readInt();

		float readTwip();

		std::string readAscii();
		std::string readFontName();

		std::vector<int8_t> readByteArray(uint32_t count);
		std::vector<int16_t> readShortArray(uint32_t count);
		std::vector<int32_t> readIntArray(uint32_t count);

	public:
		void load(const std::string& filePath);
		void save(const std::string& filePath);

	private:
		void loadInternal(const std::string& filePath, bool isTextureFile);
		void loadTags();

		void saveInternal(const std::string& filePath, bool isTextureFile);
		void saveTags(bool isTextureFile);

	private:
		ByteStream* m_stream = nullptr;

		uint16_t m_shapesCount = 0;
		uint16_t m_movieClipsCount = 0;
		uint16_t m_texturesCount = 0;
		uint16_t m_textFieldsCount = 0;
		uint16_t m_movieClipModifiersCount = 0;

		std::vector<Shape> m_shapes;
		std::vector<MovieClip> m_movieClips;
		std::vector<SWFTexture> m_textures;
		std::vector<TextField> m_textFields;
		std::vector<MovieClipModifier> m_movieClipModifiers;

		std::vector<std::string> m_fontNames;
		std::vector<MatrixBank> m_matrixBanks;

		bool m_useExternalTexture = false;
		bool m_useUncommonResolutionTexture = false;
		bool m_useHalfScaleTexture = false;

		std::string m_filePath;
		std::string m_uncommonResolutionTexturePath;

		std::string m_halfScaleFileSuffix = "_lowres";
		std::string m_UncommonResolutionFileSuffix = "_highres";
	};
}
