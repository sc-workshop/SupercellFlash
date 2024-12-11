#pragma once

#include "flash/types/SWFContainer.hpp"
#include "flash/types/SWFString.hpp"
#include "core/io/buffer_stream.h"

#include <execution>

namespace sc::flash
{
	class SupercellSWF2CompileTable
	{
	public:
		static void inline load_chunk(SupercellSWF& swf, const SC2::DataStorage* storage, const std::function<void(SupercellSWF&, const SC2::DataStorage*, const uint8_t*)>& reader)
		{
			uint32_t data_size = swf.stream.read_unsigned_int();
			reader(swf, storage, (uint8_t*)swf.stream.data() + swf.stream.position());
			swf.stream.seek(data_size, wk::Stream::SeekMode::Add);
		};

		template<class T>
		static void inline save_chunk(SupercellSWF& swf, flatbuffers::FlatBufferBuilder& builder)
		{
			builder.Finish();
		}

	public:
		SWFVector<SWFString> strings;
		SWFVector<wk::RectF> rectangles;

		wk::BufferStream frame_elements;
		wk::BufferStream bitmaps;

	public:
		uint32_t get_string_ref(const SWFString& target)
		{
			auto it = std::find(std::execution::par_unseq, strings.begin(), strings.end(), target);
			if (it != strings.end())
			{
				return std::distance(strings.begin(), it);
			}
			else
			{
				uint32_t result = strings.size();
				strings.push_back(target);
				return result;
			}
		}
	};
}