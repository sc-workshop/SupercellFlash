#pragma once

#include <stdint.h>
#include <filesystem>

#include "types/SWFStream.hpp"

// SWF Objects
#include "objects/ExportName.h"
#include "objects/MatrixBank.h"

#include "objects/SWFTexture.h"
#include "objects/Shape.h"
#include "objects/MovieClip.h"
#include "objects/TextField.h"
#include "objects/MovieClipModifier.h"

#define MULTIRES_DEFAULT_SUFFIX "_highres"
#define LOWRES_DEFAULT_SUFFIX "_lowres"

namespace sc
{
	class SupercellSWF
	{
	public:
		SupercellSWF() {};
		virtual ~SupercellSWF() = default;

	public:
		fs::path current_file;

		SWFVector<ExportName> exports;
		SWFVector<MatrixBank> matrixBanks;

		SWFVector<SWFTexture> textures;
		SWFVector<Shape> shapes;
		SWFVector<MovieClip> movieclips;
		SWFVector<TextField> textfields;
		SWFVector<MovieClipModifier> movieclip_modifiers;

	public:
		virtual void load(const std::filesystem::path& filePath);
		bool load_internal(const std::filesystem::path& filepath, bool is_texture);

		virtual void save(const fs::path& filepath, SWFStream::Signature signature);
		void save_internal(bool is_texture, bool is_lowres);

		SWFStream stream;

	protected:
		bool load_tags();

		void save_tags();
		void save_textures(bool has_data, bool is_lowres);

	public:
		uint16_t GetDisplayObjectID(SWFString& name);
		DisplayObject& GetDisplayObjectByID(uint16_t id);
		MovieClip& GetDisplayObjectByName(SWFString& name);

	public:
		// Saves all textures to _tex.sc if true
		bool use_external_texture = false;
		bool use_multi_resolution = false;
		bool use_low_resolution = true;
		bool use_precision_matrix = false;

		// Saves all textures to zktx files if true and use_external_texture is true
		bool use_external_texture_files = true;

		// Saves custom properties in MovieClips
		bool save_custom_property = true;

		bool low_memory_usage_mode = false;

		SWFString multi_resolution_suffix = MULTIRES_DEFAULT_SUFFIX;
		SWFString low_resolution_suffix = LOWRES_DEFAULT_SUFFIX;
	};
}
