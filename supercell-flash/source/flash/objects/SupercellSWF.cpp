#include "SupercellSWF.h"
#include "SupercellSWF2.h"

#include "flash/flash_tags.h"
#include <cmath>

#include "flash/SC2/FileDescriptor_generated.h"

namespace fs = std::filesystem;

namespace sc
{
	namespace flash {
#pragma region Loading
		void SupercellSWF::load(const fs::path& filepath)
		{
			current_file = filepath;
			use_external_texture = load_internal(filepath, false);

			if (use_external_texture) load_external_texture();

			stream.clear();
		}

		bool SupercellSWF::load_internal(const std::filesystem::path& filepath, bool is_texture)
		{
			stream.clear();

			wk::InputFileStream file(filepath);
			uint32_t version = SupercellSWF::GetVersion(file);
			if (version >= 5)
			{
				if (version == 6)
				{
					file.read_unsigned_short(); // always 0
				}
				load_sc2(file);
				return false;
			}
			else
			{
				file.seek(0);
				Decompressor::decompress(file, stream);
				return load_sc1(is_texture);
			}
		}

		void SupercellSWF::load_external_texture()
		{
			fs::path basename = current_file.stem();
			fs::path dirname = current_file.parent_path();

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

		void SupercellSWF::load_sc2(wk::Stream& input)
		{
			uint32_t resources_offset = 0;

			// Descriptor
			uint32_t descriptor_size = input.read_unsigned_int();
			wk::MemoryStream descriptor_data(descriptor_size);
			input.read(descriptor_data.data(), descriptor_data.length());

			const SC2::FileDescriptor* descriptor = SC2::GetFileDescriptor(descriptor_data.data());

			uint32_t shape_count = descriptor->shape_count();
			shapes.resize(shape_count);

			uint32_t movies_count = descriptor->movie_clips_count();
			movieclips.resize(movies_count);

			uint32_t texture_count = descriptor->texture_count();
			textures.resize(texture_count);

			uint32_t textFields_count = descriptor->text_fields_count();
			textfields.resize(textFields_count);

			resources_offset = descriptor->resources_offset();

			auto export_names_hash = descriptor->exports();
			if (export_names_hash)
			{
				exports.resize(export_names_hash->size());

				for (uint32_t i = 0; export_names_hash->size() > i; i++)
				{
					auto export_name_data = export_names_hash->Get(i);
					ExportName& export_name = exports[i];
					export_name.name = SWFString(
						export_name_data->name()->data(), 
						export_name_data->name()->size()
					);

					if (export_name_data->hash())
					{
						export_name.hash.resize(export_name_data->hash()->size());
						wk::Memory::copy(export_name_data->hash()->data(), export_name.hash.data(), export_name.hash.size());
					}
				}
			}

			// Compressed buffer
			{
				ZstdDecompressor decompressor;
				decompressor.decompress(input, stream);
			}

			load_sc2_internal(descriptor);
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
				exports.reserve(exports_count);

				SWFVector<uint16_t> export_ids;
				export_ids.resize(exports_count);

				for (uint16_t i = 0; exports_count > i; i++)
				{
					export_ids[i] = stream.read_unsigned_short();
				}

				for (uint16_t i = 0; exports_count > i; i++)
				{
					SWFString name;
					stream.read_string(name);

					CreateExportName(name, export_ids[i]);
				}
			}

			return load_tags();
		}

		void SupercellSWF::load_sc2_internal(const SC2::FileDescriptor* descriptor)
		{
			const SC2::DataStorage* storage = nullptr;
			{
				stream.seek(0);
				uint32_t data_storage_size = stream.read_unsigned_int();
				storage = SC2::GetDataStorage((char*)stream.data() + stream.position());
				stream.seek(data_storage_size, wk::Stream::SeekMode::Add);
				MatrixBank::load(*this, storage, descriptor->scale_precision(), descriptor->translation_precision());
			}
			
			{
				stream.seek(descriptor->resources_offset());
				using Table = SupercellSWF2CompileTable;
				Table::load_chunk(*this, storage, ExportName::load_sc2);
				Table::load_chunk(*this, storage, TextField::load_sc2);
				Table::load_chunk(*this, storage, Shape::load_sc2);
				Table::load_chunk(*this, storage, MovieClip::load_sc2);
				Table::load_chunk(*this, storage, MovieClipModifier::load_sc2);
				Table::load_chunk(*this, storage, SWFTexture::load_sc2);
			}
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

				case TAG_USE_LOW_RES_TEXTURE:
					use_low_resolution = true;
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
		void SupercellSWF::save(const fs::path& filepath, Signature signature, bool save_lowres)
		{
			current_file = filepath;

			save_internal(false, false);
			stream.save_file(filepath, signature);

			{
				fs::path basename = filepath.stem();
				fs::path dirname = filepath.parent_path();

				fs::path multi_resolution_path = dirname / fs::path(basename).concat(multi_resolution_suffix.string()).concat("_tex.sc");
				fs::path low_resolution_path = dirname / fs::path(basename).concat(low_resolution_suffix.string()).concat("_tex.sc");
				fs::path common_file_path = dirname / fs::path(basename).concat("_tex.sc");

				if (use_external_texture)
				{
					save_internal(true, false);
					stream.save_file(use_multi_resolution ? multi_resolution_path : common_file_path, signature);
				}

				if ((use_low_resolution || use_multi_resolution) && save_lowres)
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
				// Export names
				if (exports.size() >= std::numeric_limits<uint16_t>().max())
				{
					throw wk::Exception("Too many export names in use!");
				}

				// Matrix banks
				if (matrixBanks.size() > std::numeric_limits<uint16_t>().max())
				{
					throw wk::Exception("Too many matrix banks in use!");
				}

				// Textures
				if (textures.size() > std::numeric_limits<uint8_t>().max())
				{
					throw wk::Exception("Too many textures in use!");
				}

				// Shapes
				if (shapes.size() >= std::numeric_limits<uint16_t>().max())
				{
					throw wk::Exception("Too many shapes in use!");
				}

				// Movieclips
				if (movieclips.size() >= std::numeric_limits<uint16_t>().max())
				{
					throw wk::Exception("Too many movieclips in use!");
				}

				// Textfields
				if (textfields.size() >= std::numeric_limits<uint16_t>().max())
				{
					throw wk::Exception("Too many textfields in use!");
				}

				stream.write_unsigned_short((uint16_t)shapes.size());
				stream.write_unsigned_short((uint16_t)movieclips.size());
				stream.write_unsigned_short((uint16_t)textures.size());
				stream.write_unsigned_short((uint16_t)textfields.size());

				uint16_t matrices_count = 0;
				uint16_t colors_count = 0;

				if (!matrixBanks.empty())
				{
					matrices_count = matrixBanks[0].matrices.size();
					colors_count = matrixBanks[0].color_transforms.size();
				}

				stream.write_unsigned_short(matrices_count);
				stream.write_unsigned_short(colors_count);

				// unused 5 bytes
				stream.write_unsigned_byte(0);
				stream.write_int(0);

				stream.write_unsigned_short((uint16_t)exports.size());

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
				save_textures_sc1(is_texture, is_lowres);
			}

			stream.write_tag_flag(TAG_END);
		}

		void SupercellSWF::save_tags()
		{
			if (use_low_resolution)
				stream.write_tag_flag(TAG_USE_LOW_RES_TEXTURE);

			if (use_multi_resolution)
				stream.write_tag_flag(TAG_USE_MULTI_RES_TEXTURE);

			if (use_external_texture)
				stream.write_tag_flag(TAG_USE_EXTERNAL_TEXTURE);

			if (
				multi_resolution_suffix.compare(MULTIRES_DEFAULT_SUFFIX) != 0 ||
				low_resolution_suffix.compare(LOWRES_DEFAULT_SUFFIX) != 0)
			{
				size_t position = stream.write_tag_header(TAG_TEXTURE_FILE_SUFFIXES);
				stream.write_string(multi_resolution_suffix);
				stream.write_string(low_resolution_suffix);
				stream.write_tag_final(position);
			}

			save_textures_sc1(!use_external_texture, false);

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

		void SupercellSWF::save_textures_sc1(bool has_data, bool is_lowres)
		{
			for (uint16_t i = 0; textures.size() > i; i++)
			{
				SWFTexture& texture = textures[i];

				size_t position = stream.write_tag_header(texture.tag(*this, has_data));
				if (use_external_textures && has_data)
				{
					texture.encoding(SWFTexture::TextureEncoding::KhronosTexture);

					// Path String In Tag
					fs::path filename = texture.save_to_external_file(*this, i, is_lowres);

					{
						std::string texture_filename = filename.string();
						SWFString texture_path(texture_filename);
						stream.write_string(texture_path);
					}
				}
				texture.save(*this, has_data, is_lowres);
				stream.write_tag_final(position);
			}
		}

		void SupercellSWF::save_sc2(const fs::path& filepath) const
		{
			current_file = filepath;
			SupercellSWF2CompileTable table(*this);
			stream.clear();
			
			// Saving all file content to this->stream
			table.save_buffer();
			stream.seek(0);

			// Output stream
			wk::OutputFileStream file(filepath);
			file.write_unsigned_short(SC_MAGIC);						// Magic
			file.write_unsigned_int(5);									// Version
			table.save_descriptor(file);								// Descriptor
			Compressor::compress(stream, file, Signature::Zstandard);	// File Content

		}
#pragma endregion

		ExportName* SupercellSWF::GetExportName(const SWFString& name)
		{
			auto it = std::find_if(std::execution::par_unseq, exports.begin(), exports.end(), [&name](const ExportName& other)
				{
					return other.name == name;
				});

			if (it != exports.end())
			{
				return &(*it);
			}

			return nullptr;
		}

		uint16_t SupercellSWF::GetDisplayObjectID(const SWFString& name)
		{
			auto export_name = GetExportName(name);
			if (export_name)
			{
				return export_name->id;
			}

			throw new wk::Exception("Failed to get export name \"%s\"", name.data());
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

		MovieClip& SupercellSWF::GetDisplayObjectByName(const SWFString& name)
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

		void SupercellSWF::CreateExportName(const SWFString& name, uint16_t id)
		{
			auto possible_name = GetExportName(name);
			if (possible_name)
			{
				possible_name->id = id;
			}
			else
			{
				ExportName& export_name = exports.emplace_back();
				export_name.name = name;
				export_name.id = id;
			}
		}

		bool SupercellSWF::IsSC2(wk::Stream& stream)
		{
			return GetVersion(stream) >= 5;
		}

		uint32_t SupercellSWF::GetVersion(wk::Stream& stream)
		{
			if (stream.read_short() != SC_MAGIC) return 0;

			// Check for sc1 first
			uint32_t version = stream.read_unsigned_int(wk::Endian::Big);
			if (4 >= version) return version;

			uint32_t result = wk::swap_endian(version);
			if (result >= 5) {
				return result;
			}
			else
			{
				return version;
			}

			return 0;
		}
	}
}