#pragma once

#include "flash/flash_tags.h"
#include "flash/types/SWFStream.hpp"

#include <filesystem>
#include <stdint.h>

// SWF Objects
#include "ExportName.h"
#include "SWFTexture.h"
#include "flash/display_object/MovieClip.h"
#include "flash/display_object/MovieClipModifier.h"
#include "flash/display_object/Shape.h"
#include "flash/display_object/TextField.h"
#include "flash/transform/MatrixBank.h"

namespace sc::flash {
    constexpr auto MULTIRES_DEFAULT_SUFFIX = "_highres";
    constexpr auto LOWRES_DEFAULT_SUFFIX = "_lowres";

    namespace SC2 {
        struct DataStorage;
    }

    struct Sc2CompileSettings {
    public:
        enum class Version : uint32_t {
            Basic = 5,
            Unknown1 = 6
        };

    public:
        Version version = Version::Basic;

        /// @brief Save matrices with half precision if true
        bool use_half_precision_matrices = false;

        /// @brief Use short frames to optimize saved file size
        bool use_short_frames = false;

        /// @brief Compress matrix bank to optimize saved file size
        bool use_external_matrix_banks = false;
    };

    class SupercellSWF {
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
        mutable fs::path current_file;

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
        void load_external_texture();

        bool load_sc1(bool is_texture);
        void load_sc2(wk::Stream& stream);
        void load_sc2_internal(const SC2::FileDescriptor* descriptor);

        virtual void save(const fs::path& filepath, Signature signature, bool save_lowres = false);
        void save_internal(bool is_texture, bool is_lowres);

        virtual void save_sc2(const fs::path& filepath) const;

        mutable SWFStream stream;

    protected:
        bool load_tags();

        void save_tags();
        void save_textures_sc1(bool has_data, bool is_lowres);

    public:
        ExportName* GetExportName(const SWFString& name);
        uint16_t GetDisplayObjectID(const SWFString& name);
        DisplayObject& GetDisplayObjectByID(uint16_t id);
        MovieClip& GetDisplayObjectByName(const SWFString& name);
        ExportName& CreateExportName(const SWFString& name, uint16_t id);

    public:
        SWFString multi_resolution_suffix = MULTIRES_DEFAULT_SUFFIX;
        SWFString low_resolution_suffix = LOWRES_DEFAULT_SUFFIX;

        /// @brief Settings for SC2
        Sc2CompileSettings sc2_compile_settings;

        /// @brief Saves all textures to external _tex.sc file if true (SC1)
        bool use_external_texture = false;

        /// @brief Saves textures to seperate _highres_tex.sc and _lowres_tex.sc if true (SC1)
        bool use_multi_resolution = false;

        /// @brief Trying to load _lowres_tex.sc if true. If _lowres_tex.sc file does not exists, tries to generate lowres textures at runtime (SC1)
        bool use_low_resolution = false;

        /// @brief Writes matrices with greater precision (SC1)
        bool use_precision_matrix = false;

        /// @brief Compresses .ktx/.sctx (ZSTD preferably) texture if true
        bool compress_external_textures = true;

        /// @brief Saves each texture to external .ktx or .sctx file if true
        bool use_external_textures = false;

        /// @brief Writes custom properties if true (SC1)
        bool save_custom_property = true;

        /// @brief If true, tries to load lowres texture preferably
        bool low_memory_usage_mode = false;

        /// @brief Generate streaming texture when possible (SCTX feature only)
        bool use_texture_streaming = false;

    public:
        static bool IsSC2(wk::Stream& stream);
        static uint32_t GetVersion(wk::Stream& stream);
    };
}
