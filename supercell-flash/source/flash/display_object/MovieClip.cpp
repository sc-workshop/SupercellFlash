#include "MovieClip.h"
#include "flash/objects/SupercellSWF.h"

namespace sc
{
	namespace flash {
		void MovieClip::load(SupercellSWF& swf, uint8_t tag)
		{
			id = swf.stream.read_unsigned_short();
			frame_rate = swf.stream.read_unsigned_byte();
			unknown_flag = tag == TAG_MOVIE_CLIP_5;

			if (tag == TAG_MOVIE_CLIP || tag == TAG_MOVIE_CLIP_4)
				throw Exception("TAG_MOVIE_CLIP and TAG_MOVIE_CLIP_4 is unsupported");

			uint16_t frame_count = swf.stream.read_unsigned_short();
			frames.resize(frame_count);

			if (tag == TAG_MOVIE_CLIP_6)
			{
				uint8_t properties_count = swf.stream.read_unsigned_byte();

				for (uint8_t i = 0; properties_count > i; i++)
				{
					uint8_t property_type = swf.stream.read_unsigned_byte();

					switch (property_type)
					{
					case 0:
						custom_properties.emplace_back(
							std::make_any<bool>(swf.stream.read_bool())
						);
					default:
						throw Exception("Unknown or unsupported custom property");
					}
				}
			}

			int32_t frame_elements_count = swf.stream.read_int();
			frame_elements.resize(frame_elements_count);

			for (int32_t i = 0; i < frame_elements_count; i++)
			{
				MovieClipFrameElement& element = frame_elements[i];

				element.instance_index = swf.stream.read_unsigned_short();
				element.matrix_index = swf.stream.read_unsigned_short();
				element.colorTransform_index = swf.stream.read_unsigned_short();
			}

			uint16_t instance_count = swf.stream.read_unsigned_short();
			instances.resize(instance_count);

			for (int16_t i = 0; i < instance_count; i++)
			{
				instances[i].id = swf.stream.read_unsigned_short();
			}

			if (tag == TAG_MOVIE_CLIP_3 || tag >= TAG_MOVIE_CLIP_5)
			{
				for (int16_t i = 0; i < instance_count; i++)
				{
					instances[i].blend_mode = (DisplayObjectInstance::BlendMode)swf.stream.read_unsigned_byte();
				}
			}

			for (int16_t i = 0; i < instance_count; i++)
			{
				swf.stream.read_string(instances[i].name);
			}

			uint16_t frames_total = 0;
			while (true)
			{
				uint8_t frame_tag = swf.stream.read_unsigned_byte();
				int32_t frame_tag_length = swf.stream.read_int();

				if (frame_tag == 0)
					break;

				if (frame_tag_length < 0)
					throw Exception("Negative tag length");

				switch (frame_tag)
				{
				case TAG_MOVIE_CLIP_FRAME_2:
					frames[frames_total].load(swf);
					frames_total++;
					break;

				case TAG_SCALING_GRID:
				{
					RectF grid;
					grid.x = swf.stream.read_twip();
					grid.y = swf.stream.read_twip();
					grid.width = swf.stream.read_twip();
					grid.height = swf.stream.read_twip();

					scaling_grid = grid;
				}
				break;

				case TAG_MATRIX_BANK_INDEX:
					bank_index = swf.stream.read_unsigned_byte();
					break;

				default:
					swf.stream.seek(frame_tag_length, Stream::SeekMode::Add);
					break;
				}
			}
		}

		void MovieClip::save(SupercellSWF& swf) const
		{
			swf.stream.write_unsigned_short(id);
			swf.stream.write_unsigned_byte(frame_rate);
			swf.stream.write_unsigned_short(frames.size());

			if (swf.save_custom_property)
			{
				for (std::any custom_property : custom_properties)
				{
					uint8_t property_type = 0xFF;

					if (custom_property.type() == typeid(bool))
					{
						property_type = 0;
					}
					else
					{
						throw Exception("Unknown or unsupported custom property type");
					}

					swf.stream.write_unsigned_byte(property_type);
					switch (property_type)
					{
					case 0:
						swf.stream.write_bool(std::any_cast<bool>(custom_property));
					default:
						break;
					}
				}
			}

			swf.stream.write_unsigned_int(frame_elements.size());
			for (const MovieClipFrameElement& element : frame_elements)
			{
				swf.stream.write_unsigned_short(element.instance_index);
				swf.stream.write_unsigned_short(element.matrix_index);
				swf.stream.write_unsigned_short(element.colorTransform_index);
			}

			swf.stream.write_short(instances.size());

			for (const DisplayObjectInstance& instance : instances)
			{
				swf.stream.write_unsigned_short(instance.id);
			}

			for (const DisplayObjectInstance& instance : instances)
			{
				swf.stream.write_unsigned_byte((uint8_t)instance.blend_mode);
			}

			for (const DisplayObjectInstance& instance : instances)
			{
				swf.stream.write_string(instance.name);
			}

			if (bank_index != 0)
			{
				swf.stream.write_unsigned_byte(TAG_MATRIX_BANK_INDEX);
				swf.stream.write_int(1);
				swf.stream.write_unsigned_byte(bank_index);
			}

			for (const MovieClipFrame& frame : frames)
			{
				size_t position = swf.stream.write_tag_header(frame.tag(swf));
				frame.save(swf);
				swf.stream.write_tag_final(position);
			}

			if (scaling_grid != std::nullopt)
			{
				swf.stream.write_unsigned_byte(TAG_SCALING_GRID);
				swf.stream.write_int(sizeof(scaling_grid));

				swf.stream.write_int((int)(scaling_grid->x * 20.0f));
				swf.stream.write_int((int)(scaling_grid->y * 20.0f));
				swf.stream.write_int((int)(scaling_grid->width * 20.0f));
				swf.stream.write_int((int)(scaling_grid->height * 20.0f));
			}

			swf.stream.write_tag_flag(TAG_END);
		}

		uint8_t MovieClip::tag(SupercellSWF& swf) const
		{
			if (swf.save_custom_property)
			{
				return TAG_MOVIE_CLIP_6;
			}

			return unknown_flag ? TAG_MOVIE_CLIP_5 : TAG_MOVIE_CLIP_3;
		}

		bool MovieClip::is_movieclip() const
		{
			return true;
		}
	}
}