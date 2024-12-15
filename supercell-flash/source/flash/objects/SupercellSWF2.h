#pragma once

#include "flash/types/SWFContainer.hpp"
#include "flash/types/SWFString.hpp"
#include "core/io/buffer_stream.h"
#include "core/math/rect.h"

#include "flash/SC2/DataStorage_generated.h"

#include <execution>

namespace sc::flash
{
	class SupercellSWF;

	class SupercellSWF2CompileTable
	{
	public:
		SupercellSWF2CompileTable();

	public:
		static void load_chunk(SupercellSWF& swf, const SC2::DataStorage* storage, const std::function<void(SupercellSWF&, const SC2::DataStorage*, const uint8_t*)>& reader);

		template<class T>
		static void save_chunk(SupercellSWF& swf, flatbuffers::FlatBufferBuilder& builder)
		{
			builder.Finish();
		}

	public:
		SWFVector<SWFString> strings;
		SWFVector<wk::RectF> rectangles;

		SWFVector<uint32_t> exports_refs;

		wk::BufferStream frame_elements;
		wk::BufferStream bitmaps;

		flatbuffers::FlatBufferBuilder builder;

	public:
		uint32_t get_string_ref(const SWFString& string);
		uint32_t get_rect_ref(const wk::RectF& rectangle);

	public:
		void gather_resources(SupercellSWF& swf);

	};
}