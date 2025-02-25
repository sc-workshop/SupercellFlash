#pragma once

#include "flash/types/SWFContainer.hpp"
#include "flash/types/SWFString.hpp"
#include "flash/objects/SWFTexture.h"
#include "core/io/buffer_stream.h"
#include "core/math/rect.h"

#include "flash/SC2/DataStorage_generated.h"
#include "flash/SC2/FileDescriptor_generated.h"

#include <execution>

namespace sc::flash
{
	class SupercellSWF;

	class SupercellSWF2CompileTable
	{
	public:
		using RefT = uint32_t;

		template<typename T>
		using RefArray = std::vector<T>;

		// font_name, text, style_path
		using TextfieldRef = std::tuple<RefT, RefT, RefT>;

		// children_names, frame_elements_offset, frame_labels, scaling_grid
		using MovieClipRef = std::tuple<std::optional<RefArray<RefT>>, RefT, RefArray<RefT>, std::optional<RefT>>;

	public:
		SupercellSWF2CompileTable(const SupercellSWF& swf);

	public:
		static void load_chunk(SupercellSWF& swf, const SC2::DataStorage* storage, const std::function<void(SupercellSWF&, const SC2::DataStorage*, const uint8_t*)>& reader);
		static const float get_precision_multiplier(SC2::Precision);

	public:
		// Matrix precision
		SC2::Precision scale_presicion = SC2::Precision::Default;
		SC2::Precision translation_precision = SC2::Precision::Default;

		// Resource palette
		SWFVector<SWFString> strings;
		SWFVector<SC2::Typing::Rect> rectangles;

		// Exports
		RefArray<RefT> exports_ref_indices;

		// Movieclips
		std::vector<uint16_t> frame_elements_indices;
		RefArray<MovieClipRef> movieclips_ref_indices;

		// Shapes
		wk::BufferStream bitmaps_buffer;
		RefArray<RefT> bitmaps_offsets;

		// Textfiels
		RefArray<TextfieldRef> textfields_ref_indices;

		// Root
		flatbuffers::FlatBufferBuilder builder;

	public:
		uint32_t get_string_ref(const SWFString& string);
		uint32_t get_rect_ref(const wk::RectF& rectangle);

	private:
		template<typename T>
		size_t flush_builder(flatbuffers::Offset<T> root_off)
		{
			builder.FinishSizePrefixed(root_off);
			auto buffer = builder.GetBufferSpan();
			size_t buffer_size = buffer.size_bytes();
			swf.stream.write(buffer.data(), buffer_size);

			builder.Clear();

			return buffer_size;
		}

		void save_header();
		void save_exports();
		void save_textFields();
		void save_shapes();
		void save_movieClips();
		void save_modifiers();
		uint32_t save_textures();

		flatbuffers::Offset<sc::flash::SC2::TextureData> create_texture(const SWFTexture& texture, uint32_t index, bool is_lowres);

	public:
		void gather_resources();
		void save_buffer();
		void save_descriptor(wk::Stream& stream);

	private:
		const SupercellSWF& swf;

		uint32_t header_offset = 0;
		uint32_t data_offset = 0;
		uint32_t textures_length = 0;
	};
}