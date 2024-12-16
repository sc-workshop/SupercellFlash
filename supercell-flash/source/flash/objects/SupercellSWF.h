#pragma once

#include <stdint.h>
#include <filesystem>

#include "flash/flash_tags.h"
#include "flash/types/SWFStream.hpp"

// SWF Objects
#include "SWFTexture.h"
#include "ExportName.h"

#include "flash/transform/MatrixBank.h"

#include "flash/display_object/Shape.h"
#include "flash/display_object/MovieClip.h"
#include "flash/display_object/TextField.h"
#include "flash/display_object/MovieClipModifier.h"

namespace sc
{
	namespace flash {
		constexpr auto MULTIRES_DEFAULT_SUFFIX = "_highres";
		constexpr auto LOWRES_DEFAULT_SUFFIX = "_lowres";

		namespace SC2
		{
			class DataStorage;
		}

		class SupercellSWF
		{
			// Type traits
		public:
			using ExportsArray = SWFVector<ExportName, uint32_t>;
			using MatrixBankArray = SWFVector<MatrixBank, uint32_t>;
			using TextureArray = SWFVector<SWFTexture, uint32_t>;
			using ShapeArray = SWFVector<Shape, uint32_t>;
			using MovieClipArray = SWFVector<MovieClip, uint32_t>;
			using TextFieldArray = SWFVector<TextField, uint32_t>;
			using ModifiersArray = SWFVector<MovieClipModifier>;

		public:
			SupercellSWF() {};
			virtual ~SupercellSWF() = default;
			SupercellSWF(const SupercellSWF&) = default;
			SupercellSWF(SupercellSWF&&) = default;
			SupercellSWF& operator=(const SupercellSWF&) = default;
			SupercellSWF& operator=(SupercellSWF&&) = default;

		public:
			fs::path current_file;

			ExportsArray exports;
			MatrixBankArray matrixBanks;

			TextureArray textures;
			ShapeArray shapes;
			MovieClipArray movieclips;
			TextFieldArray textfields;
			ModifiersArray movieclip_modifiers;

		public:
			virtual void load(const std::filesystem::path& filePath);
			bool load_internal(const std::filesystem::path& filepath, bool is_texture);

			bool load_sc1(bool is_texture);
			void load_sc2(wk::Stream& stream);
			void load_sc2_internal(uint32_t header_offset, uint32_t resources_offset);

			virtual void save(const fs::path& filepath, Signature signature);
			void save_internal(bool is_texture, bool is_lowres);

			virtual void save_sc2(const fs::path& filepath) const;

			mutable SWFStream stream;

		protected:
			bool load_tags();

			void save_tags();
			void save_textures(bool has_data, bool is_lowres);

		public:
			ExportName* GetExportName(SWFString& name);
			uint16_t GetDisplayObjectID(SWFString& name);
			DisplayObject& GetDisplayObjectByID(uint16_t id);
			MovieClip& GetDisplayObjectByName(SWFString& name);
			void CreateExportName(SWFString& name, uint16_t id);

		public:
			// Saves all textures to _tex.sc if true
			bool use_external_texture = false;

			bool use_multi_resolution = false;

			// Save low-resolution texture variant
			bool use_low_resolution = true;

			// Write matrices with greater precision
			bool use_precision_matrix = false;

			// Compresses ktx/sctx texture files
			bool compress_external_textures = true;

			// Saves textures to ktx/sctx
			bool use_external_textures = false;

			// Saves custom properties in MovieClips
			bool save_custom_property = true;

			// Load only lowres files if available
			bool low_memory_usage_mode = false;

			SWFString multi_resolution_suffix = MULTIRES_DEFAULT_SUFFIX;
			SWFString low_resolution_suffix = LOWRES_DEFAULT_SUFFIX;

		public:
			static bool IsSC2(wk::Stream& stream);
		};
	}
}
