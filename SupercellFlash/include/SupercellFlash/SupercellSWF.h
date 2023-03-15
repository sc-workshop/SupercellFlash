#pragma once

#include <cstdio>
#include <cstdint>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>

#include "io/SWFStream.h"

#include "SupercellFlash/objects/Shape.h"
#include "SupercellFlash/objects/MovieClip.h"
#include "SupercellFlash/objects/SWFTexture.h"
#include "SupercellFlash/objects/TextField.h"
#include "SupercellFlash/objects/MovieClipModifier.h"
#include "SupercellFlash/objects/ExportName.h"

#include "SupercellFlash/transformation/MatrixBank.h"

#include "SupercellFlash/Tags.h"

#define MULTIRES_DEFAULT_SUFFIX "_highres"
#define LOWRES_DEFAULT_SUFFIX "_lowres"

namespace sc
{
	class SupercellSWF
	{
	public:
		SupercellSWF();
		~SupercellSWF();

		SWFStream stream;

		// Vectors with objects
	public:
		std::vector<MatrixBank> matrixBanks;

		std::vector<SWFTexture> textures;
		std::vector<Shape> shapes;
		std::vector<MovieClip> movieClips;
		std::vector<TextField> textFields;
		std::vector<MovieClipModifier> movieClipModifiers;
		std::vector<ExportName> exports;

		// Class functions
	public:
		void load(const std::string& filePath);
		void loadTexture(const std::string& filePath);

		void save(const std::string& filepath, CompressionSignature signature);
		void saveTexture(const std::string& filepath, bool isLowres, CompressionSignature signature);

		// Getters for class members
	public:
		bool useExternalTexture() { return m_useExternalTexture; }

		bool useMultiResTexture() { return m_useMultiResTexture; }
		bool useLowResTexture() { return m_useLowResTexture; }

		std::string multiResFileSuffix() { return m_multiResFileSuffix; }
		std::string lowResFileSuffix() { return m_lowResFileSuffix; }

		// Setters for class members
	public:
		void useExternalTexture(bool status) { m_useExternalTexture = status; }

		void useMultiResTexture(bool status) { m_useMultiResTexture = status; }
		void useLowResTexture(bool status) { m_useLowResTexture = status; }

		void multiResFileSuffix(std::string postfix) { m_multiResFileSuffix = postfix; }
		void lowResFileSuffix(std::string postfix) { m_lowResFileSuffix = postfix; }

	private:
		bool loadInternal(bool isTexture);
		bool loadTags();

		void saveInternal();
		void saveTags(
			uint16_t shapeCount,
			uint16_t movieClipsCount,
			uint16_t texturesCount,
			uint16_t textFieldsCount
		);

		void initMatrixBank(uint16_t matricesCount, uint16_t colorTransformsCount);

	private:
		bool m_useExternalTexture = false;

		bool m_useMultiResTexture = false;
		bool m_useLowResTexture = false;

		std::string m_multiResFileSuffix = MULTIRES_DEFAULT_SUFFIX;
		std::string m_lowResFileSuffix = LOWRES_DEFAULT_SUFFIX;
	};
}
