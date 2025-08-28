#pragma once

#include <flash/flash.h>
#include <filesystem>

#include <core/stb/stb.h>

#include <nlohmann/json.hpp>

#include "SpriteData.hpp"

using namespace sc::flash;
using json = nlohmann::ordered_json;

namespace sc
{
	namespace sctex
	{
		bool CompareUV(float x, float y, float epsilon = 0.001f) {
			if (fabs(x - y) < epsilon)
				return true;
			return false;
		}

		bool CommandEqual(const ShapeDrawBitmapCommand& a, const ShapeDrawBitmapCommand& b)
		{
			if (a.texture_index != b.texture_index) return false;
			if (a.vertices.size() != b.vertices.size()) return false;

			for (uint16_t i = 0; a.vertices.size() > i; i++)
			{
				const ShapeDrawBitmapCommandVertex& vertex_a = a.vertices[i];
				const ShapeDrawBitmapCommandVertex& vertex_b = b.vertices[i];

				if (!CompareUV(vertex_a.u, vertex_b.u)) return false;
				if (!CompareUV(vertex_a.v, vertex_b.v)) return false;
			}

			return  true;
		}
		void PremultiplyToStraight(wk::RawImage& image)
		{
			for (uint16_t w = 0; image.width() > w; w++) {
				for (uint16_t h = 0; image.height() > h; h++) {
					wk::ColorRGBA& pixel = image.at<wk::ColorRGBA>(w, h);

					float factor = ((float)pixel.a / 255.f);
					pixel.r = (uint8_t)(std::clamp(pixel.r / factor, 0.f, 255.f));
					pixel.g = (uint8_t)(std::clamp(pixel.g / factor, 0.f, 255.f));
					pixel.b = (uint8_t)(std::clamp(pixel.b / factor, 0.f, 255.f));
				}
			}
		}

		void StraightToPremultiply(wk::RawImage& image)
		{
			assert(image.depth() == wk::Image::PixelDepth::RGBA8 && "Image should be RGBA8 before converting");

			for (uint16_t w = 0; image.width() > w; w++) {
				for (uint16_t h = 0; image.height() > h; h++) {
					wk::ColorRGBA& pixel = image.at<wk::ColorRGBA>(w, h);

					float factor = ((float)pixel.a / 255.f);
					pixel.r = (uint8_t)((float)pixel.r * factor);
					pixel.g = (uint8_t)((float)pixel.g * factor);
					pixel.b = (uint8_t)((float)pixel.b * factor);
				}
			}
		}
	}
}

namespace sc
{
	namespace sctex
	{
		class SWFFile : public SupercellSWF
		{
		public:
			SWFFile() {};

			SWFFile(std::filesystem::path path, bool load_all = false)
			{
				current_file = path;

				wk::InputFileStream file(path);

				uint32_t version = SupercellSWF::GetVersion(file);
				if (version >= 5)
				{
					std::cout << "File is loaded as a SC2" << std::endl;

					if (version == 6)
					{
						file.read_unsigned_short(); // always 0
					}

					sc2_compile_settings.version = Sc2CompileSettings::Version(version);
					load_sc2(file);
				}
				else
				{
					load_sc1(path, load_all);
				}
			}

		public:
			// If the file is a real texture, only the texture tags are loaded, otherwise the entire file is loaded.
			void load_sc1(std::filesystem::path path, bool load_all = false)
			{
				stream.open_file(path);

				// Path Check
				if (path.string().find("_tex") != std::string::npos)
				{
					load_texures_from_binary();

					return;
				}

				bool isTexture = true;

				// Tags check
				{
					uint8_t tag = stream.read_unsigned_byte();
					int32_t tag_length = stream.read_int();

					while (stream.length() > stream.position()) {
						switch (tag)
						{
						case TAG_END:
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
							if (tag_length <= 0 || tag_length + stream.length() >= stream.position()) {
								isTexture = false;
								break;
							};

							stream.seek(tag_length, wk::Stream::SeekMode::Add);
							break;
						default:
							isTexture = false;
							break;
						}

						if (!isTexture)
							break;
					}

					if (isTexture)
						std::cout << "File is loaded as a texture file because the first tag is a texture" << std::endl;

					stream.seek(0);
				}

				if (isTexture) {
					load_texures_from_binary();
					return;
				}

				// Whole file loading
				{
					std::cout << "File is loaded as a default asset file because it did not pass all texture checks" << std::endl;

					if (load_all)
					{
						stream.clear();
						load(path);
					}
					else
					{
						// Skip of all count fields
						stream.seek(17);

						// export names skip
						uint16_t exports_count = stream.read_unsigned_short();

						for (uint16_t i = 0; exports_count > i; i++)
						{
							stream.read_unsigned_short();
						}

						for (uint16_t i = 0; exports_count > i; i++)
						{
							uint8_t length = stream.read_unsigned_byte();
							stream.seek(length, wk::Stream::SeekMode::Add);
						}

						load_texures_from_binary();
					}
				}
			}

			void load_texures_from_binary()
			{
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
						textures.emplace_back().load(*this, tag, true);
						break;

					default:
						stream.seek(tag_length, wk::Stream::SeekMode::Add);
						break;
					}
				}
			}

			void save_sprites_to_folder(std::filesystem::path output_path)
			{
				SpriteData data;
				std::vector<ShapeDrawBitmapCommand> commands;

				for (SWFTexture& texture : textures)
				{
					data.textures.emplace_back(texture.image()->width(), texture.image()->height());
				}

				for (Shape& shape : shapes)
				{
					for (ShapeDrawBitmapCommand& command : shape.commands)
					{
						auto commnand_it = std::find_if(commands.begin(), commands.end(), [&command](const ShapeDrawBitmapCommand& current) {return CommandEqual(command, current); });
						if (commands.size() == 0 || commnand_it == commands.end())
						{
							auto& sprite = data.sprites.emplace_back();
							sprite.name = std::string("sprite_") + std::to_string(commands.size());
							sprite.texture_index = command.texture_index;
							auto image = textures[command.texture_index].image();
							for (auto& vertex : command.vertices)
							{
								sprite.vertices.emplace_back((uint16_t)(vertex.u * image->width()), (uint16_t)(vertex.v * image->height()));
							}

							commands.push_back(command);
						}
					}
				}

				std::filesystem::path basename = output_path.stem();
				std::filesystem::path data_file = output_path / fs::path(basename).concat(".sctex");

				data.Save(data_file);

				return;
			}

			void save_textures_to_folder(std::filesystem::path output_path)
			{
				json texture_infos = json::array();

				for (uint16_t i = 0; textures.size() > i; i++)
				{
					SWFTexture& texture = textures[i];

					// Texture Info
					{
						std::string encoding;
						{
							switch (texture.encoding())
							{
							case SWFTexture::TextureEncoding::KhronosTexture:
								encoding = "khronos";
								break;
							case SWFTexture::TextureEncoding::Raw:
								encoding = "raw";
								break;
							default:
								break;
							}
						}

						std::string pixel_type = "RGBA8";

						switch (texture.pixel_format())
						{
						case SWFTexture::PixelFormat::RGBA4:
							pixel_type = "RGBA4";
							break;
						case SWFTexture::PixelFormat::RGB5_A1:
							pixel_type = "RGB5_A1";
							break;
						case SWFTexture::PixelFormat::RGB565:
							pixel_type = "RGB565";
							break;
						case SWFTexture::PixelFormat::LUMINANCE8_ALPHA8:
							pixel_type = "LUMINANCE8_ALPHA8";
							break;
						case SWFTexture::PixelFormat::LUMINANCE8:
							pixel_type = "LUMINANCE8";
							break;

						case SWFTexture::PixelFormat::RGBA8:
						default:
							break;
						}

						std::string filtering = "LINEAR_NEAREST";

						switch (texture.filtering)
						{
						case SWFTexture::Filter::LINEAR_MIPMAP_NEAREST:
							filtering = "LINEAR_MIPMAP_NEAREST";
							break;
						case SWFTexture::Filter::NEAREST_NEAREST:
							filtering = "NEAREST_NEAREST";
							break;
						case SWFTexture::Filter::LINEAR_NEAREST:
						default:
							break;
						}

						json texture_info = {
							{"Encoding", encoding},
							{"PixelFormat", pixel_type},
							{"Filtering", filtering},
							{"Linear", texture.linear()},
						};

						texture_infos.push_back(texture_info);
					}

					// Texture Image
					std::filesystem::path basename = output_path.stem();
					std::filesystem::path output_image_path = output_path / basename.concat("_").concat(std::to_string(i)).concat(".png");
					wk::OutputFileStream output_image(output_image_path);

					wk::stb::ImageFormat format = wk::stb::ImageFormat::PNG;

					auto image = texture.raw_image();
					if (image->channels() == 2 || image->channels() == 4) {
						if (image->depth() != wk::Image::PixelDepth::RGBA8) {
							wk::RawImageRef decoded = wk::CreateRef<wk::RawImage>(image->width(), image->height(), wk::Image::PixelDepth::RGBA8);
							image->copy(*decoded);
							image = decoded;
						}
						PremultiplyToStraight(*image);
					}
					wk::stb::write_image(*image, format, output_image);

					std::cout << "Decoded texture: " << output_image_path << std::endl;
				}

				std::string serialized_data = texture_infos.dump(4);

				wk::OutputFileStream file_info(output_path / output_path.stem().concat(".json"));
				file_info.write(serialized_data.data(), serialized_data.size());
			}

			void load_textures_from_folder(std::filesystem::path input)
			{
				current_file = input;
				std::filesystem::path basename = input.stem();
				std::filesystem::path texture_info_path = std::filesystem::path(input / basename.concat(".json"));

				// Texture Info Parsing
				json texture_infos = json::array({});

				if (!std::filesystem::exists(texture_info_path))
				{
					std::cout << "Texture info file does not exist. Default settings will be used instead" << std::endl;
				}
				else
				{
					std::ifstream file(texture_info_path);
					texture_infos = json::parse(file);
				}

				// Texture Images path gather
				SWFVector<std::filesystem::path> texture_images_paths;

				for (auto const& file_descriptor : std::filesystem::directory_iterator(input))
				{
					std::filesystem::path filepath = file_descriptor.path();
					std::filesystem::path file_extension = filepath.extension();

					if (file_extension == ".png")
					{
						texture_images_paths.push_back(filepath);
					}
				}

				//Texture Converting
				for (uint16_t i = 0; texture_images_paths.size() > i; i++)
				{
					// Image Loading
					wk::RawImageRef image;
					wk::InputFileStream image_file(texture_images_paths[i]);
					wk::stb::load_image(image_file, image);
					if (image->channels() == 4 || image->channels() == 2) {
						if (image->depth() != wk::Image::PixelDepth::RGBA8) {
							wk::RawImageRef decoded = wk::CreateRef<wk::RawImage>(image->width(), image->height(), wk::Image::PixelDepth::RGBA8);
							image->copy(*decoded);
							image = decoded;
						}
						StraightToPremultiply(*image);
					}

					wk::SharedMemoryStream image_data(image->data(), image->data_length());

					// Image Converting
					SWFTexture texture;
					texture.load_from_image(*image);

					if (texture_infos.size() > i)
					{
						json texture_info = texture_infos[i];

						SWFTexture::PixelFormat texture_type = SWFTexture::PixelFormat::RGBA8;

						if (texture_info["PixelFormat"] == "RGBA4")
						{
							texture_type = SWFTexture::PixelFormat::RGBA4;
						}
						else if (texture_info["PixelFormat"] == "RGB5_A1")
						{
							texture_type = SWFTexture::PixelFormat::RGB5_A1;
						}
						else if (texture_info["PixelFormat"] == "RGB565")
						{
							texture_type = SWFTexture::PixelFormat::RGB565;
						}
						else if (texture_info["PixelFormat"] == "LUMINANCE8_ALPHA8")
						{
							texture_type = SWFTexture::PixelFormat::LUMINANCE8_ALPHA8;
						}
						else if (texture_info["PixelFormat"] == "LUMINANCE8")
						{
							texture_type = SWFTexture::PixelFormat::LUMINANCE8;
						}

						if (texture_info["Filtering"] == "LINEAR_NEAREST")
						{
							texture.filtering = SWFTexture::Filter::LINEAR_NEAREST;
						}
						else if (texture_info["Filtering"] == "LINEAR_MIPMAP_NEAREST")
						{
							texture.filtering = SWFTexture::Filter::LINEAR_MIPMAP_NEAREST;
						}
						else if (texture_info["Filtering"] == "NEAREST_NEAREST")
						{
							texture.filtering = SWFTexture::Filter::NEAREST_NEAREST;
						}

						if (texture_info["Encoding"] == "khronos")
						{
							texture.encoding(SWFTexture::TextureEncoding::KhronosTexture);
						}
						else if (texture_info["Encoding"] == "raw")
						{
							texture.encoding(SWFTexture::TextureEncoding::Raw);
							texture.pixel_format(texture_type);
						}
					}

					// Texture Insert
					if (textures.size() <= i)
					{
						textures.push_back(texture);
					}
					else
					{
						textures[i] = texture;
					}

					std::cout << "Processed texture: " << texture_images_paths[i] << std::endl;
				}
			}
		};
	}
}