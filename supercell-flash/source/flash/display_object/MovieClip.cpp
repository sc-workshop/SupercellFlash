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
				throw wk::Exception("TAG_MOVIE_CLIP and TAG_MOVIE_CLIP_4 is unsupported");

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
						break;
					default:
						throw wk::Exception("Unknown or unsupported custom property");
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
			childrens.resize(instance_count);

			for (int16_t i = 0; i < instance_count; i++)
			{
				childrens[i].id = swf.stream.read_unsigned_short();
			}

			if (tag == TAG_MOVIE_CLIP_3 || tag >= TAG_MOVIE_CLIP_5)
			{
				for (int16_t i = 0; i < instance_count; i++)
				{
					childrens[i].blend_mode = (DisplayObjectInstance::BlendMode)swf.stream.read_unsigned_byte();
				}
			}

			for (int16_t i = 0; i < instance_count; i++)
			{
				swf.stream.read_string(childrens[i].name);
			}

			uint16_t frames_total = 0;
			while (true)
			{
				uint8_t frame_tag = swf.stream.read_unsigned_byte();
				int32_t frame_tag_length = swf.stream.read_int();

				if (frame_tag == 0)
					break;

				if (frame_tag_length < 0)
					throw wk::Exception("Negative tag length");

				switch (frame_tag)
				{
				case TAG_MOVIE_CLIP_FRAME_2:
					frames[frames_total].load(swf);
					frames_total++;
					break;

				case TAG_SCALING_GRID:
				{
					wk::RectF grid;
					grid.left = swf.stream.read_twip();
					grid.top = swf.stream.read_twip();
					float width = swf.stream.read_twip();
					float height = swf.stream.read_twip();

					grid.right = grid.left + width;
					grid.bottom = grid.top + height;

					scaling_grid = grid;
				}
				break;

				case TAG_MATRIX_BANK_INDEX:
					bank_index = swf.stream.read_unsigned_byte();
					break;

				default:
					swf.stream.seek(frame_tag_length, wk::Stream::SeekMode::Add);
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
				swf.stream.write_unsigned_byte(custom_properties.size());
				for (std::any custom_property : custom_properties)
				{
					uint8_t property_type = 0xFF;

					if (custom_property.type() == typeid(bool))
					{
						property_type = 0;
					}
					else
					{
						throw wk::Exception("Unknown or unsupported custom property type");
					}

					swf.stream.write_unsigned_byte(property_type);
					switch (property_type)
					{
					case 0:
						swf.stream.write_bool(std::any_cast<bool>(custom_property));
						break;
					default:
						break;
					}
				}
			}

			swf.stream.write_unsigned_int(frame_elements.size());
			write_frame_elements_buffer(swf.stream);

			swf.stream.write_short(childrens.size());

			for (const DisplayObjectInstance& children : childrens)
			{
				swf.stream.write_unsigned_short(children.id);
			}

			for (const DisplayObjectInstance& children : childrens)
			{
				swf.stream.write_unsigned_byte((uint8_t)children.blend_mode);
			}

			for (const DisplayObjectInstance& children : childrens)
			{
				swf.stream.write_string(children.name);
			}

			if (bank_index != 0)
			{
				swf.stream.write_unsigned_byte(TAG_MATRIX_BANK_INDEX);
				swf.stream.write_int(1);
				swf.stream.write_unsigned_byte((uint8_t)bank_index);
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
				swf.stream.write_int(sizeof(int) * 4);

				swf.stream.write_twip(scaling_grid->left);
				swf.stream.write_twip(scaling_grid->top);
				swf.stream.write_twip(std::abs(scaling_grid->left - scaling_grid->right));
				swf.stream.write_twip(std::abs(scaling_grid->top - scaling_grid->bottom));
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

		void MovieClip::load_sc2(SupercellSWF& swf, const SC2::DataStorage* storage, const uint8_t* data)
		{
			auto movieclips_data = SC2::GetMovieClips(data);

			auto movieclips_vector = movieclips_data->movieclips();
			if (!movieclips_vector) return;

			uint16_t movieclips_count = (uint16_t)movieclips_vector->size();
			swf.movieclips.reserve(movieclips_count);

			auto strings_vector = storage->strings();
			auto elements_vector = storage->movieclips_frame_elements();
			auto rectangles_vector = storage->rectangles();

			for (uint16_t i = 0; movieclips_count > i; i++)
			{
				auto movieclip_data = movieclips_vector->Get(i);
				MovieClip& movieclip = swf.movieclips[i];

				movieclip.id = movieclip_data->id();
				movieclip.frame_rate = movieclip_data->framerate();

				movieclip.bank_index = movieclip_data->matrix_bank_index();
				movieclip.unknown_flag = (bool)movieclip_data->unknown_bool();

				{
					auto scaling_grid = movieclip_data->scaling_grid_index();
					if (scaling_grid.has_value())
					{
						auto rectangle = rectangles_vector->Get(scaling_grid.value());
						movieclip.scaling_grid = wk::RectF(
							rectangle->left(),
							rectangle->top(),
							rectangle->right(),
							rectangle->bottom()
						);
					}
				}

				auto children_ids_vector = movieclip_data->children_ids();
				auto children_blending_vector = movieclip_data->children_blending();
				auto children_names_vector = movieclip_data->children_name_ref_ids();

				uint16_t children_count = 0;
				if (children_ids_vector)
				{
					children_count = (uint16_t)children_ids_vector->size();
					movieclip.childrens.resize(children_count);

					for (uint16_t c = 0; children_count > c; c++)
					{
						movieclip.childrens[c].id = children_ids_vector->Get(c);
					}
				}
				

				if (children_blending_vector)
				{
					for (uint16_t c = 0; children_blending_vector->size() > c && children_count > c; c++)
					{
						movieclip.childrens[c].blend_mode = (DisplayObjectInstance::BlendMode)children_blending_vector->Get(c);
					}
				}

				if (children_names_vector)
				{
					for (uint16_t c = 0; children_names_vector->size() > c && children_count > c; c++)
					{
						movieclip.childrens[c].name = SWFString(
							strings_vector->Get(
								children_names_vector->Get(c)
							)->c_str()
						);
					}
				}

				auto frames_vector = movieclip_data->frames();
				auto short_frames_vector = movieclip_data->short_frames();
				uint16_t frames_count = (uint16_t)movieclip_data->frames_count();
				movieclip.frames.reserve(frames_count);
				swf.sc2_compile_settings.use_short_frames |= short_frames_vector != nullptr;

				if (frames_vector)
				{
					for (auto frame_data : *frames_vector)
					{
						MovieClipFrame& frame = movieclip.frames.emplace_back();
						uint32_t label_id = frame_data->label_ref_id();
						frame.elements_count = frame_data->used_transform();
						frame.label = SWFString(
							strings_vector->Get(label_id)->c_str()
						);
					}
				}
				else if (short_frames_vector)
				{
					for (auto frame_data : *short_frames_vector)
					{
						MovieClipFrame& frame = movieclip.frames.emplace_back();
						frame.elements_count = frame_data->used_transform();
					}
				}


				uint32_t elements_count = 0;
				uint32_t elements_offset = movieclip_data->frame_elements_offset();
				for (MovieClipFrame& frame : movieclip.frames)
				{
					elements_count += frame.elements_count;
				}

				movieclip.frame_elements.reserve(elements_count);
				for (uint32_t e = 0; elements_count > e; e++)
				{
					MovieClipFrameElement& element = movieclip.frame_elements.emplace_back();

					element.instance_index = elements_vector->Get(elements_offset++);
					element.matrix_index = elements_vector->Get(elements_offset++);
					element.colorTransform_index = elements_vector->Get(elements_offset++);
				}
			}
		}

		void MovieClip::write_frame_elements_buffer(wk::Stream& stream) const
		{
			for (const MovieClipFrameElement& element : frame_elements)
			{
				stream.write_unsigned_short(element.instance_index);
				stream.write_unsigned_short(element.matrix_index);
				stream.write_unsigned_short(element.colorTransform_index);
			}
		}
	}
}