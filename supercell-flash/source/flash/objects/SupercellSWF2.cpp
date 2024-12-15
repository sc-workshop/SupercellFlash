#include "SupercellSWF2.h"

#include "flash/objects/SupercellSWF.h"

using namespace flatbuffers;

namespace sc::flash
{
	// SupercellSWF2CompileTable

	SupercellSWF2CompileTable::SupercellSWF2CompileTable()
	{
		builder = FlatBufferBuilder(1024 * 1024, nullptr, false);
		builder.TrackMinAlign(16);
	}

	void SupercellSWF2CompileTable::load_chunk(SupercellSWF& swf, const SC2::DataStorage* storage, const std::function<void(SupercellSWF&, const SC2::DataStorage*, const uint8_t*)>& reader)
	{
		uint32_t data_size = swf.stream.read_unsigned_int();
		reader(swf, storage, (uint8_t*)swf.stream.data() + swf.stream.position());
		swf.stream.seek(data_size, wk::Stream::SeekMode::Add);
	};

	uint32_t SupercellSWF2CompileTable::get_string_ref(const SWFString& target)
	{
		auto& it = std::find(std::execution::par_unseq, strings.begin(), strings.end(), target);
		if (it != strings.end())
		{
			return (uint32_t)std::distance(strings.begin(), it);
		}
		else
		{
			uint32_t result = strings.size();
			strings.push_back(target);
			return result;
		}
	}

	uint32_t SupercellSWF2CompileTable::get_rect_ref(const wk::RectF& rect)
	{
		auto& it = std::find_if(std::execution::par_unseq, rectangles.begin(), rectangles.end(), [&rect](const wk::RectF other)
			{
				return rect.x == other.x && rect.y == other.y && rect.width == other.width && rect.height == other.height;
			});

		if (it != rectangles.end())
		{
			return (uint32_t)std::distance(rectangles.begin(), it);
		}
		else
		{
			uint32_t result = rectangles.size();
			rectangles.push_back(rect);
			return result;
		}
	}
}