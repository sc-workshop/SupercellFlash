#include "SupercellSWF.h"

#include "flash/flash_tags.h"
#include <cmath>

// SC2
#include "SC2/DataStorage_generated.h"
#include "SC2/ExportNames_generated.h"
#include "SC2/TextFields_generated.h"
#include "SC2/Shapes_generated.h"
#include "SC2/MovieClips_generated.h"
#include "SC2/MovieClipModifiers_generated.h"
#include "SC2/Textures_generated.h"

namespace fs = std::filesystem;

namespace sc
{
	namespace flash {
#pragma region Loading
		void SupercellSWF::load(const fs::path& filepath)
		{
			current_file = filepath;
			use_external_texture = load_internal(filepath, false);

			if (use_external_texture)
			{
				fs::path basename = filepath.stem();
				fs::path dirname = filepath.parent_path();

				fs::path multi_resolution_path = dirname / fs::path(basename).concat(multi_resolution_suffix.string()).concat("_tex.sc");
				fs::path low_resolution_path = dirname / fs::path(basename).concat(low_resolution_suffix.string()).concat("_tex.sc");
				fs::path common_file_path = dirname / fs::path(basename).concat("_tex.sc");

				if (low_memory_usage_mode && use_low_resolution && fs::exists(low_resolution_path))
				{
					load_internal(low_resolution_path, true);
				}
				else if (use_multi_resolution && fs::exists(multi_resolution_path))
				{
					load_internal(multi_resolution_path, true);
				}
				else if (fs::exists(common_file_path))
				{
					load_internal(common_file_path, true);
				}
				else
				{
					throw wk::Exception("Failed to load external texture file");
				}
			}

			stream.clear();
		}

		bool SupercellSWF::load_internal(const std::filesystem::path& filepath, bool is_texture)
		{
			stream.clear();

			wk::InputFileStream file(filepath);
			if (file.read_short() == SC_MAGIC && file.read_unsigned_int() == 5)
			{
				uint32_t metadata_size = file.read_unsigned_int();

				// TODO: Metadata
				file.seek(metadata_size, wk::Stream::SeekMode::Add);

				ZstdDecompressor decompressor;
				decompressor.decompress(file, stream);

				save_as_sc2 = true;
				load_sc2();
				return false;
			}
			else
			{
				file.seek(0);
				Decompressor::decompress(file, stream);
				return load_sc1(is_texture);
			}
		}

		bool SupercellSWF::load_sc1(bool is_texture)
		{
			stream.seek(0);

			// Reading .sc file
			if (!is_texture)
			{
				uint16_t shapes_count = stream.read_unsigned_short();
				shapes.resize(shapes_count);

				uint16_t movie_clips_count = stream.read_unsigned_short();
				movieclips.resize(movie_clips_count);

				uint16_t textures_count = stream.read_unsigned_short();
				textures.resize(textures_count);

				uint16_t textfield_count = stream.read_unsigned_short();
				textfields.resize(textfield_count);

				uint16_t matrices_count = stream.read_unsigned_short();
				uint16_t colors_count = stream.read_unsigned_short();
				matrixBanks.resize(1, MatrixBank(matrices_count, colors_count));

				stream.seek(5, wk::Stream::SeekMode::Add); // unused

				uint16_t exports_count = stream.read_unsigned_short();
				exports.resize(exports_count);

				for (ExportName& export_name : exports)
				{
					export_name.id = stream.read_unsigned_short();
				}

				for (ExportName& export_name : exports)
				{
					stream.read_string(export_name.name);
				}
			}

			return load_tags();
		}

		void SupercellSWF::load_sc2_matrix_banks(const SC2::DataStorage* storage)
		{
			auto matrix_banks_vector = storage->matrix_banks();
			// Return if empty
			if (!matrix_banks_vector) return;

			uint32_t matrix_bank_count = matrix_banks_vector->size();
			matrixBanks.reserve(matrix_bank_count);

			for (uint32_t i = 0; matrix_bank_count > i; i++)
			{
				auto bank_data = matrix_banks_vector->Get(i);
				MatrixBank& bank = matrixBanks.emplace_back();

				auto matrices_vector = bank_data->matrices();
				auto colors_vector = bank_data->colors();

				if (matrices_vector)
				{
					uint16_t matrices_count = (uint16_t)matrices_vector->size();
					bank.matrices.reserve(matrices_count);

					for (uint16_t m = 0; matrices_count > m; m++)
					{
						auto matrix_data = matrices_vector->Get(m);
						Matrix2D& matrix = bank.matrices.emplace_back();
						matrix.a = matrix_data->a(); matrix.b = matrix_data->b(); matrix.c = matrix_data->c(); matrix.d = matrix_data->d();
						matrix.tx = matrix_data->tx(); matrix.ty = matrix_data->ty();
					}
				}

				if (colors_vector)
				{
					uint16_t colors_count = (uint16_t)colors_vector->size();
					bank.color_transforms.reserve(colors_count);

					for (uint16_t c = 0; colors_count > c; c++)
					{
						auto color_data = colors_vector->Get(c);
						ColorTransform& color = bank.color_transforms.emplace_back();
						color.add.r = color_data->r_add(); color.add.g = color_data->g_add(); color.add.b = color_data->b_add();
						color.multiply.r = color_data->r_mul(); color.multiply.g = color_data->g_mul(); color.multiply.b = color_data->b_mul();
						color.alpha = color_data->alpha();
					}
				}
			}
		}

		void SupercellSWF::load_sc2_export_names(const SC2::DataStorage* storage, const uint8_t* data)
		{
			auto exports_data = SC2::GetExportNames(data);
			auto exports_ids = exports_data->object_ids();
			auto exports_name_ref_ids = exports_data->name_ref_ids();

			// Return if some of vectors are empty
			if (!exports_ids || !exports_name_ref_ids) return;
			if (exports_ids->size() != exports_name_ref_ids->size())
			{
				throw wk::Exception();
			}

			auto strings_vector = storage->strings();
			uint16_t export_names_count = (uint16_t)exports_ids->size();
			exports.reserve(export_names_count);
			for (uint16_t i = 0; export_names_count > i; i++)
			{
				ExportName& export_name = exports.emplace_back();
				export_name.id = exports_ids->Get(i);
				export_name.name = SWFString(
					strings_vector->Get(
						exports_name_ref_ids->Get(i)
					)->c_str()
				);
			}
		}

		void SupercellSWF::load_sc2_textfields(const SC2::DataStorage* storage, const uint8_t* data)
		{
			auto textfields_data = SC2::GetTextFields(data);

			auto textfields_vector = textfields_data->textfields();
			if (!textfields_vector) return;

			auto strings_vector = storage->strings();
			uint16_t textfields_count = (uint16_t)textfields_vector->size();
			textfields.reserve(textfields_count);

			for (uint16_t i = 0; textfields_count > i; i++)
			{
				auto textfield_data = textfields_vector->Get(i);
				TextField& textfield = textfields.emplace_back();

				textfield.id = textfield_data->id();
				textfield.font_name = SWFString(
					strings_vector->Get(
						textfield_data->font_name_ref_id()
					)->c_str()
				);

				textfield.left = textfield_data->left();
				textfield.right = textfield_data->right();
				textfield.top = textfield_data->top();
				textfield.bottom = textfield_data->bottom();

				textfield.font_color = textfield_data->font_color();
				textfield.outline_color = textfield_data->outline_color();

				textfield.text = SWFString(
					strings_vector->Get(
						textfield_data->text_ref_id()
					)->c_str()
				);

				textfield.font_vertical_align = TextField::get_vertical_align(textfield_data->align());
				textfield.font_horizontal_align = TextField::get_horizontal_align(textfield_data->align());

				textfield.font_size = textfield_data->font_size();
			}
		}

		void SupercellSWF::load_sc2_shapes(const SC2::DataStorage* storage, const uint8_t* data)
		{
			auto shapes_data = SC2::GetShapes(data);

			auto shapes_vector = shapes_data->shapes();
			if (!shapes_vector) return;

			auto shape_vertex_buffer = storage->shapes_bitmap_poins();

			uint16_t shapes_count = (uint16_t)shapes_vector->size();
			shapes.reserve(shapes_count);

			for (uint16_t i = 0; shapes_count > i; i++)
			{
				auto shape_data = shapes_vector->Get(i);
				Shape& shape = shapes.emplace_back();
				shape.id = shape_data->id();

				auto commands_vector = shape_data->commands();
				if (!commands_vector) continue;

				uint32_t commands_count = commands_vector->size();
				shape.commands.reserve(commands_count);

				for (uint32_t c = 0; commands_count > c; c++)
				{
					auto command_data = commands_vector->Get(c);
					ShapeDrawBitmapCommand& command = shape.commands.emplace_back();
					command.texture_index = command_data->texture_index();

					uint32_t vertex_count = command_data->points_count();
					command.vertices.reserve(vertex_count);

					uint32_t vertex_offset = command_data->points_offset();
					for (uint32_t v = 0; vertex_count > v; v++)
					{
						const uint8_t* vertex_data = shape_vertex_buffer->data() + ((vertex_offset + v) * 12);
						ShapeDrawBitmapCommandVertex& vertex = command.vertices.emplace_back();

						vertex.x = *(const float*)vertex_data;
						vertex.y = *(const float*)(vertex_data + sizeof(float));

						vertex.u = (float)(*(const uint16_t*)(vertex_data + (sizeof(float) * 2))) / 0xFFFF;
						vertex.v = (float)(*(const uint16_t*)(vertex_data + (sizeof(float) * 2) + sizeof(uint16_t))) / 0xFFFF;
					}

					command.create_triangle_indices(true);
				}
			}
		}

		void SupercellSWF::load_sc2_movieclip(const SC2::DataStorage* storage, const uint8_t* data)
		{
			auto movieclips_data = SC2::GetMovieClips(data);

			auto movieclips_vector = movieclips_data->movieclips();
			if (!movieclips_vector) return;

			uint16_t movieclips_count = (uint16_t)movieclips_vector->size();
			movieclips.reserve(movieclips_count);

			auto strings_vector = storage->strings();
			auto movieclip_elements_vector = storage->movieclips_frame_elements();
			auto rectangles_vector = storage->rectangles();

			for (uint16_t i = 0; movieclips_count > i; i++)
			{
				auto movieclip_data = movieclips_vector->Get(i);
				MovieClip& movieclip = movieclips.emplace_back();

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

				if (!children_ids_vector)
				{
					// No need to process frames or something else if movieclip have no childrens
					continue;
				}

				uint16_t children_count = (uint16_t)children_ids_vector->size();
				movieclip.childrens.resize(children_count);

				for (uint16_t c = 0; children_count > c; c++)
				{
					movieclip.childrens[c].id = children_ids_vector->Get(c);
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
				uint16_t frames_count = (uint16_t)movieclip_data->frames_count();
				movieclip.frames.reserve(frames_count);

				for (uint16_t f = 0; frames_count > f; f++)
				{
					auto frame_data = frames_vector->Get(f);
					MovieClipFrame& frame = movieclip.frames.emplace_back();

					frame.elements_count = frame_data->used_transform();
					frame.label = SWFString(
						strings_vector->Get(
							frame_data->label_ref_id()
						)->c_str()
					);
				}

				uint32_t elements_count = 0;
				uint32_t elements_offset = movieclip_data->frame_elements_offset() * sizeof(uint16_t);
				for (MovieClipFrame& frame : movieclip.frames)
				{
					elements_count += frame.elements_count;
				}

				movieclip.frame_elements.reserve(elements_count);

				for (uint32_t e = 0; elements_count > e; e++)
				{
					const uint8_t* element_data = movieclip_elements_vector->data() + elements_offset;
					MovieClipFrameElement& element = movieclip.frame_elements.emplace_back();

					element.instance_index = *(const uint16_t*)element_data;
					element.matrix_index = *(const uint16_t*)(element_data + sizeof(uint16_t));
					element.colorTransform_index = *(const uint16_t*)(element_data + sizeof(uint16_t) * 2);

					elements_offset += sizeof(MovieClipFrameElement);
				}
			}
		}

		void SupercellSWF::load_sc2_movieclip_modifiers(const SC2::DataStorage*, const uint8_t* data)
		{
			auto modifiers_data = SC2::GetMovieClipModifiers(data);

			auto modifiers_vector = modifiers_data->modifiers();
			if (!modifiers_vector) return;

			uint16_t modifiers_count = (uint16_t)modifiers_vector->size();
			movieclip_modifiers.reserve(modifiers_count);

			for (uint16_t i = 0; modifiers_count > i; i++)
			{
				auto modifier_data = modifiers_vector->Get(i);
				MovieClipModifier& modifier = movieclip_modifiers.emplace_back();

				modifier.id = modifier_data->id();
				modifier.type = (MovieClipModifier::Type)modifier_data->type();
			}
		}

		void SupercellSWF::load_sc2_textures(const SC2::DataStorage*, const uint8_t* data)
		{
			auto textures_data = SC2::GetTextures(data);

			auto textures_vector = textures_data->textures();
			if (!textures_vector) return;

			uint32_t texture_count = textures_vector->size();

			for (uint32_t i = 0; texture_count > i; i++)
			{
				auto texture_set_data = textures_vector->Get(i);
				auto texture_data = low_memory_usage_mode && texture_set_data->lowres() ? texture_set_data->lowres() : texture_set_data->highres();
				SWFTexture& texture = textures.emplace_back();

				// Hardcode Khronos texture for now
				wk::MemoryStream texture_stream((uint8_t*)texture_data->data()->data(), texture_data->data()->size());
				texture.load_from_khronos_texture(texture_stream);
			}
		}

		void SupercellSWF::load_sc2()
		{
			stream.seek(0);

			uint32_t data_storage_size = stream.read_unsigned_int();
			auto data_storage = SC2::GetDataStorage((char*)stream.data() + stream.position());
			stream.seek(data_storage_size, wk::Stream::SeekMode::Add);

			load_sc2_matrix_banks(data_storage);

			load_sc2_chunk(data_storage, &SupercellSWF::load_sc2_export_names);
			load_sc2_chunk(data_storage, &SupercellSWF::load_sc2_textfields);
			load_sc2_chunk(data_storage, &SupercellSWF::load_sc2_shapes);
			load_sc2_chunk(data_storage, &SupercellSWF::load_sc2_movieclip);
			load_sc2_chunk(data_storage, &SupercellSWF::load_sc2_movieclip_modifiers);
			load_sc2_chunk(data_storage, &SupercellSWF::load_sc2_textures);
		}

		bool SupercellSWF::load_tags()
		{
			bool has_external_texture = false;

			uint16_t shapes_loaded = 0;
			uint16_t movieclips_loaded = 0;
			uint16_t textures_loaded = 0;
			uint16_t textfields_loaded = 0;
			uint8_t banks_loaded = 0;
			uint16_t matrices_loaded = 0;
			uint16_t colors_loaded = 0;
			uint16_t modifiers_loaded = 0;

			while (true)
			{
				uint8_t tag = stream.read_unsigned_byte();
				int32_t tag_length = stream.read_int();

				if (tag == TAG_END)
					break;

				if (tag_length < 0)
					throw wk::Exception("Negative tag length");

				switch (tag)
				{
				case TAG_USE_MULTI_RES_TEXTURE:
					use_multi_resolution = true;
					break;

				case TAG_DISABLE_LOW_RES_TEXTURE:
					use_low_resolution = false;
					break;

				case TAG_USE_EXTERNAL_TEXTURE:
					has_external_texture = true;
					break;

				case TAG_TEXTURE_FILE_SUFFIXES:
					stream.read_string(multi_resolution_suffix);
					stream.read_string(low_resolution_suffix);
					break;

				case TAG_TEXTURE:
				case TAG_TEXTURE_2:
				case TAG_TEXTURE_3:
				case TAG_TEXTURE_4:
				case TAG_TEXTURE_5:
				case TAG_TEXTURE_6:
				case TAG_TEXTURE_7:
				case TAG_TEXTURE_8:
				case TAG_TEXTURE_9:
				case TAG_TEXTURE_10:
					if (textures.size() < textures_loaded) {
						throw wk::Exception("Trying to load too many textures");
					}

					textures[textures_loaded].load(*this, tag, !has_external_texture);
					textures_loaded++;
					break;

				case TAG_MOVIE_CLIP_MODIFIERS_COUNT: {
					uint16_t modifiers_count = stream.read_unsigned_short();
					movieclip_modifiers.resize(modifiers_count);
					break;
				}

				case TAG_MOVIE_CLIP_MODIFIER:
				case TAG_MOVIE_CLIP_MODIFIER_2:
				case TAG_MOVIE_CLIP_MODIFIER_3:
					movieclip_modifiers[modifiers_loaded].load(*this, tag);
					modifiers_loaded++;
					break;

				case TAG_SHAPE:
				case TAG_SHAPE_2:
					if (shapes.size() < shapes_loaded) {
						throw wk::Exception("Trying to load too many Shapes");
					}

					shapes[shapes_loaded].load(*this, tag);
					shapes_loaded++;
					break;

				case TAG_TEXT_FIELD:
				case TAG_TEXT_FIELD_2:
				case TAG_TEXT_FIELD_3:
				case TAG_TEXT_FIELD_4:
				case TAG_TEXT_FIELD_5:
				case TAG_TEXT_FIELD_6:
				case TAG_TEXT_FIELD_7:
				case TAG_TEXT_FIELD_8:
					if (textfields.size() < textfields_loaded) {
						throw wk::Exception("Trying to load too many TextFields");
					}

					textfields[textfields_loaded].load(*this, tag);
					textfields_loaded++;
					break;

				case TAG_MATRIX_BANK:
					matrices_loaded = 0;
					colors_loaded = 0;
					banks_loaded++;
					{
						uint16_t matrix_count = stream.read_unsigned_short();
						uint16_t colors_count = stream.read_unsigned_short();
						matrixBanks.emplace_back(matrix_count, colors_count);
					}
					break;

				case TAG_MATRIX_2x3:
				case TAG_MATRIX_2x3_2:
					matrixBanks[banks_loaded].matrices[matrices_loaded].load(*this, tag);
					matrices_loaded++;
					break;

				case TAG_COLOR_TRANSFORM:
					matrixBanks[banks_loaded].color_transforms[colors_loaded].load(*this);
					colors_loaded++;
					break;

				case TAG_MOVIE_CLIP:
				case TAG_MOVIE_CLIP_2:
				case TAG_MOVIE_CLIP_3:
				case TAG_MOVIE_CLIP_4:
				case TAG_MOVIE_CLIP_5:
				case TAG_MOVIE_CLIP_6:
					if (movieclips.size() < movieclips_loaded) {
						throw wk::Exception("Trying to load too many MovieClips");
					}

					movieclips[movieclips_loaded].load(*this, tag);
					movieclips_loaded++;
					break;

				default:
					stream.seek(tag_length, wk::Stream::SeekMode::Add);
					break;
				}
			}

			return has_external_texture;
		}

#pragma endregion

#pragma region Saving
		void SupercellSWF::save(const fs::path& filepath, Signature signature)
		{
			current_file = filepath;

			if (matrixBanks.size() > std::numeric_limits<uint16_t>().max())
			{
				throw wk::Exception("Too many matrix banks in use");
			}

			if (textures.size() > std::numeric_limits<uint8_t>().max())
			{
				throw wk::Exception("Too many textures in use");
			}

			if (matrixBanks.size() == 0) {
				matrixBanks.resize(1);
			}

			save_internal(false, false);
			stream.save_file(filepath, signature);

			if (use_external_texture) {
				fs::path basename = filepath.stem();
				fs::path dirname = filepath.parent_path();

				fs::path multi_resolution_path = dirname / fs::path(basename).concat(multi_resolution_suffix.string()).concat("_tex.sc");
				fs::path low_resolution_path = dirname / fs::path(basename).concat(low_resolution_suffix.string()).concat("_tex.sc");
				fs::path common_file_path = dirname / fs::path(basename).concat("_tex.sc");

				save_internal(true, false);
				stream.save_file(use_multi_resolution ? multi_resolution_path : common_file_path, signature);

				if (use_low_resolution)
				{
					save_internal(true, true);
					stream.save_file(low_resolution_path, signature);
				}
			}
		}

		void SupercellSWF::save_internal(bool is_texture, bool is_lowres)
		{
			if (!is_texture)
			{
				stream.write_unsigned_short(shapes.size());
				stream.write_unsigned_short(movieclips.size());
				stream.write_unsigned_short(textures.size());
				stream.write_unsigned_short(textfields.size());

				stream.write_unsigned_short(matrixBanks[0].matrices.size());
				stream.write_unsigned_short(matrixBanks[0].color_transforms.size());

				// unused 5 bytes
				stream.write_unsigned_byte(0);
				stream.write_int(0);

				stream.write_unsigned_short(exports.size());

				for (const ExportName& export_name : exports)
				{
					stream.write_unsigned_short(export_name.id);
				}

				for (const ExportName& export_name : exports)
				{
					stream.write_string(export_name.name);
				}

				save_tags();
			}
			else
			{
				save_textures(is_texture, is_lowres);
			}

			stream.write_tag_flag(TAG_END);
		}

		void SupercellSWF::save_tags()
		{
			if (use_external_texture) {
				if (
					multi_resolution_suffix.compare(MULTIRES_DEFAULT_SUFFIX) != 0 ||
					low_resolution_suffix.compare(LOWRES_DEFAULT_SUFFIX) != 0) {
					size_t position = stream.write_tag_header(TAG_TEXTURE_FILE_SUFFIXES);
					stream.write_string(multi_resolution_suffix);
					stream.write_string(low_resolution_suffix);
					stream.write_tag_final(position);
				}

				if (!use_low_resolution)
					stream.write_tag_flag(TAG_DISABLE_LOW_RES_TEXTURE);

				if (use_multi_resolution)
					stream.write_tag_flag(TAG_USE_MULTI_RES_TEXTURE);

				stream.write_tag_flag(TAG_USE_EXTERNAL_TEXTURE);
			}

			save_textures(!use_external_texture, false);

			if (movieclip_modifiers.size() > 0) {
				stream.write_unsigned_byte(TAG_MOVIE_CLIP_MODIFIERS_COUNT); // Tag
				stream.write_int(sizeof(uint16_t)); // Tag Size
				stream.write_unsigned_short(movieclip_modifiers.size());

				for (const MovieClipModifier& modifier : movieclip_modifiers)
				{
					size_t position = stream.write_tag_header(modifier.tag(*this));
					modifier.save(*this);
					stream.write_tag_final(position);
				}
			}

			for (const Shape& shape : shapes)
			{
				size_t position = stream.write_tag_header(shape.tag(*this));
				shape.save(*this);
				stream.write_tag_final(position);
			}

			for (const TextField& textField : textfields)
			{
				size_t position = stream.write_tag_header(textField.tag(*this));
				textField.save(*this);
				stream.write_tag_final(position);
			}

			for (uint8_t i = 0; matrixBanks.size() > i; i++)
			{
				const MatrixBank& bank = matrixBanks[i];

				if (i != 0)
				{
					stream.write_unsigned_byte(bank.tag(*this)); // Tag
					stream.write_int(sizeof(uint16_t) * 2); // Tag Size
					stream.write_unsigned_short(bank.matrices.size());
					stream.write_unsigned_short(bank.color_transforms.size());
				}

				for (const Matrix2D& matrix : bank.matrices)
				{
					size_t position = stream.write_tag_header(matrix.tag(*this));
					matrix.save(*this);
					stream.write_tag_final(position);
				}

				for (const ColorTransform& color : bank.color_transforms)
				{
					size_t position = stream.write_tag_header(color.tag(*this));
					color.save(*this);
					stream.write_tag_final(position);
				}
			}

			for (const MovieClip& movieclip : movieclips)
			{
				size_t position = stream.write_tag_header(movieclip.tag(*this));
				movieclip.save(*this);
				stream.write_tag_final(position);
			}
		}

		void SupercellSWF::save_textures(bool has_data, bool is_lowres)
		{
			for (uint16_t i = 0; textures.size() > i; i++)
			{
				SWFTexture& texture = textures[i];

				size_t position = stream.write_tag_header(texture.tag(*this, has_data));
				if (use_external_texture_files && has_data)
				{
					texture.encoding(SWFTexture::TextureEncoding::KhronosTexture);

					// Path String In Tag
					fs::path output_filepath = current_file.parent_path();
					output_filepath /= current_file.stem();
					if (is_lowres)
					{
						output_filepath += low_resolution_suffix.string();
					}
					output_filepath += "_";
					output_filepath += std::to_string(i);
					output_filepath += ".zktx";

					{
						std::string texture_filename = output_filepath.filename().string();
						SWFString texture_path(texture_filename);
						stream.write_string(texture_path);
					}

					wk::BufferStream input_data;
					texture.save_buffer(input_data, is_lowres);

					wk::OutputFileStream output_file(output_filepath);

					ZstdCompressor::Props props;
					ZstdCompressor cctx(props);

					input_data.seek(0);
					cctx.compress(input_data, output_file);
				}
				texture.save(*this, has_data, is_lowres);
				stream.write_tag_final(position);
			}
		}
#pragma endregion

		uint16_t SupercellSWF::GetDisplayObjectID(SWFString& name)
		{
			for (ExportName& exportName : exports)
			{
				if (exportName.name == name)
				{
					return exportName.id;
				}
			}

			throw new wk::Exception("Failed to get Display Object");
		}

		DisplayObject& SupercellSWF::GetDisplayObjectByID(uint16_t id)
		{
			for (Shape& shape : shapes)
			{
				if (shape.id == id)
				{
					return shape;
				}
			}

			for (TextField& textfield : textfields)
			{
				if (textfield.id == id)
				{
					return textfield;
				}
			}

			for (MovieClipModifier& modifier : movieclip_modifiers)
			{
				if (modifier.id == id)
				{
					return modifier;
				}
			}

			for (MovieClip& movie : movieclips)
			{
				if (movie.id == id)
				{
					return movie;
				}
			}

			throw new wk::Exception("Failed to get Display Object");
		}

		MovieClip& SupercellSWF::GetDisplayObjectByName(SWFString& name)
		{
			uint16_t id = GetDisplayObjectID(name);
			for (MovieClip& movie : movieclips)
			{
				if (movie.id == id)
				{
					return movie;
				}
			}

			throw new wk::Exception("Failed to get Display Object");
		}
	}
}