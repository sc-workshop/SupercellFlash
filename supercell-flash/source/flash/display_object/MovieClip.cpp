#include "MovieClip.h"
#include "flash/objects/SupercellSWF.h"

// TODO(pavidloq): move it to some const maybe? (we are on C++, not C)
#define COMPRESSED_CLIP_DATA_MAX_SIZE 4096

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
			if (swf.sc2_compile_settings.version == Sc2CompileSettings::Version::Unknown1)
			{
				load_sc2_v6(swf, storage, data);
				return;
			}
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

		void MovieClip::load_sc2_v6(SupercellSWF& swf, const SC2::DataStorage* storage, const uint8_t* data)
		{
			auto movieclips_data = SC2::GetMovieClipV6s(data);

			auto movieclips_vector = movieclips_data->movieclips();
			if (!movieclips_vector) return;

			uint16_t movieclips_count = (uint16_t)movieclips_vector->size();
			swf.movieclips.reserve(movieclips_count);

			auto strings_vector = storage->strings();
			auto elements_vector = storage->movieclips_frame_elements();
			auto rectangles_vector = storage->rectangles();

			uint16_t* out_decompressed = new uint16_t[COMPRESSED_CLIP_DATA_MAX_SIZE];

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

				// TODO: short frames support? (idk)
				auto frames_vector = movieclip_data->frames();
				// auto short_frames_vector = movieclip_data->short_frames();
				uint16_t frames_count = (uint16_t)movieclip_data->frames_count();
				movieclip.frames.reserve(frames_count);
				// swf.sc2_compile_settings.use_short_frames |= short_frames_vector != nullptr;

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

				// FIXME: scope below
				if (movieclip_data->frame_data_offset() != -1)
				{
					uint8_t* compressed_frame_data =
						&swf.matrixBanks[movieclip_data->matrix_bank_index()].
						compressed_clip_data[movieclip_data->frame_data_offset()];
					uint8_t* compressed_frame_data_end = compressed_frame_data - movieclip_data->frame_data_offset() + swf.matrixBanks[movieclip_data->matrix_bank_index()].compressed_clip_size;
					uint16_t total_elements_count = *(uint16_t*)(compressed_frame_data + 4);
					
					for (int frame_index = 0; frame_index < movieclip.frames.size(); frame_index++)
					{
						unsigned char* this_frame_data = compressed_frame_data + frame_index * 8 + 8;
						// unsigned char* last_frame_data = compressed_frame_data + (movieclip.frames.size() - 1) * 8 + 8;
						unsigned short* element_data = (unsigned short*)(compressed_frame_data + *(int*)(this_frame_data));
						if ((unsigned char*)element_data >= compressed_frame_data_end) abort();
						unsigned short* element_data_start = (unsigned short*)((unsigned char*)element_data + 2 * *(unsigned short*)(this_frame_data + 4));
						unsigned short* element_data_end = (unsigned short*)((unsigned char*)element_data + 2 * *(unsigned short*)(this_frame_data + 6));
						int decompressed_size_shorts = decode_compressed_frame_data(element_data, element_data_start, element_data_end, out_decompressed);
						if (decompressed_size_shorts > COMPRESSED_CLIP_DATA_MAX_SIZE) abort();  // FIXME: bro :skull:
						
						for (int k = 0; k < decompressed_size_shorts / 3; k++) {
							MovieClipFrameElement& element = movieclip.frame_elements.emplace_back();

							element.instance_index = out_decompressed[k * 3 + 0];
							element.matrix_index = out_decompressed[k * 3 + 1];
							element.colorTransform_index = out_decompressed[k * 3 + 2];
							
							if (element.colorTransform_index != 0xFFFF && element.colorTransform_index >= swf.matrixBanks[movieclip.bank_index].color_transforms.size() ||
								element.instance_index >= children_count) {
								abort();  // FIXME: bro :skull:
							}
							if (element.matrix_index != 0xFFFF && element.matrix_index >= swf.matrixBanks[movieclip.bank_index].matrices.size()) {
								abort();  // FIXME: bro :skull:
							}
						}
						
						movieclip.frames[frame_index].elements_count = decompressed_size_shorts / 3;
					}
				}
				else
				{
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

			// TODO: wrap it into smart pointer maybe to avoid possible memory leaks?
			delete[] out_decompressed;
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
		int MovieClip::decode_compressed_frame_data(unsigned short* element_data, unsigned short* element_data_start, unsigned short* element_data_end, unsigned short* result)
		{
			unsigned short* v7 = result;
			unsigned short* v8 = &result[COMPRESSED_CLIP_DATA_MAX_SIZE];
			int v6 = 0;
			if (element_data_start == element_data)
			{
				memcpy(result, element_data_start, (unsigned char*)element_data_end - (unsigned char*)element_data_start);
				return element_data_end - element_data_start;
			}
			else if (element_data_start != element_data_end || v6)
			{
				while (element_data_start != element_data_end && v8 - v7 >= 6)
				{
					if (element_data_start >= element_data_end) abort();
					unsigned short v17 = element_data_start[0];
					unsigned short v13 = element_data[0];
					unsigned short v14 = element_data[1];
					unsigned short v16 = element_data[2];
					if ((v6 & 1) != 0)
					{
						*v7 = v13;
						v7[1] = v14;
						v7[2] = v16;
						v7 += 3;
						element_data += 3;
						v6 >>= 1;
					}
					else
					{
						v6 >>= 1;
						if ((v17 & 3) != 0)
						{
							int v23 = (v17 & 7) - 1;
							// printf("switch: %d\n", v23 + 1);
							switch (v17 & 7)
							{
							case 1:
								*v7 = v13;
								v23 = v14;
								v7[1] = ((int32_t)(v17 << (32 - 13 - 3)) >> (32 - 13)) + v14;
								v7[2] = v16;
								v7 += 3;
								element_data += 3;
								++element_data_start;
								break;
							case 2:
								*v7 = v13;
								v23 = v14;
								v7[1] = ((int32_t)(v17 << (32 - 4 - 3)) >> (32 - 4)) + v14;
								v23 = v16;
								v7[2] = ((int32_t)(v17 << (32 - 9 - 7)) >> (32 - 9)) + v16;
								v7 += 3;
								element_data += 3;
								++element_data_start;
								break;
							case 3:
								*v7 = v13;
								v7[1] = element_data_start[1] + v14;
								v23 = v16;
								v7[2] = ((int32_t)(v17 << (32 - 13 - 3)) >> (32 - 13)) + v16;
								v7 += 3;
								element_data += 3;
								element_data_start += 2;
								break;
							case 5:
								*v7 = v13;
								v7[1] = v14;
								v7[2] = v16;
								v7 += 3;
								element_data += 3;
								++element_data_start;
								v6 = (int32_t)v17 >> 3;
								break;
							case 6:
								element_data += 3 * ((int32_t)(v17 << (32 - 13 - 3)) >> (32 - 13));
								++element_data_start;
								break;
							case 7:
								*v7 = ((int32_t)(v17 << (32 - 12 - 3)) >> (32 - 12));
								v7[1] = element_data_start[1];
								v7[2] = element_data_start[2];
								v7 += 3;
								element_data_start += 3;
								if ((v17 & 0x8000) != 0)
									v23 = 3;
								else
									v23 = 0;
								element_data += v23;
								break;
							default:
								continue;
							}
						}
						else
						{
							*v7 = v13;
							v7[1] = ((int32_t)(v17 << (32 - 7 - 2)) >> (32 - 7)) + v14;
							v7[2] = v16;
							v7[3] = element_data[3];
							v7[4] = ((int32_t)(v17 << (32 - 7 - 9)) >> (32 - 7)) + element_data[4];
							v7[5] = element_data[5];
							v7 += 6;
							element_data += 6;
							++element_data_start;
							v6 >>= 1;
						}
					}
				}
				if (element_data_start == element_data_end)
				{
					while (v6 && v8 - v7 >= 3)
					{
						if ((v6 & 1) == 0)
						{
							abort();
						}
						*v7 = *element_data;
						v7[1] = element_data[1];
						v7[2] = element_data[2];
						v7 += 3;
						element_data += 3;
						v6 >>= 1;
					}
				}
				if (v7 > v8)
				{
					abort();
				}
				return v7 - result;
			}
			else
			{
				abort();
			}
		}
	}
}
