#include <vector>
#include <filesystem>
#include <flash/flash.h>
#include <core/math/point.h>

using namespace sc::flash;

namespace sc
{
	namespace sctex
	{
		class SpriteData
		{
		public:
			class TextureInstance
			{
			public:
				uint16_t width;
				uint16_t height;
			public:
				TextureInstance(uint16_t width, uint16_t height) : width(width), height(height) {}
			};

			class SpriteInstance
			{
			public:
				SWFString name;
				uint8_t texture_index;
				SWFVector<wk::Point_t<uint16_t>> vertices;
			};

		private:
			SWFStream m_stream;

		public:
			SWFVector<TextureInstance> textures;
			SWFVector<SpriteInstance> sprites;

		public:
			void Save(std::filesystem::path path)
			{
				m_stream.clear();

				WriteV1();
				m_stream.save_file(path, sc::flash::Signature::Zstandard);

				m_stream.clear();
			}

			void Load(std::filesystem::path path)
			{
				m_stream.clear();
				m_stream.open_file(path);

				uint8_t version = m_stream.read_unsigned_byte();
				switch (version)
				{
				case 1:
					ReadV1();
					break;
				default:
					break;
				}

				m_stream.clear();
			}
		private:
			void WriteV1()
			{
				// Version
				m_stream.write_byte(1);

				// Count
				m_stream.write_unsigned_short(textures.size());
				m_stream.write_unsigned_short(sprites.size());

				// Data
				for (TextureInstance& texture : textures)
				{
					m_stream.write_unsigned_short(texture.width);
					m_stream.write_unsigned_short(texture.height);
				}

				for (SpriteInstance& sprite : sprites)
				{
					m_stream.write_string(sprite.name);
					m_stream.write_unsigned_byte(sprite.texture_index);
					m_stream.write_unsigned_short(sprite.vertices.size());

					for (auto& vertex : sprite.vertices)
					{
						m_stream.write_unsigned_short(vertex.u);
						m_stream.write_unsigned_short(vertex.v);
					}
				}
			}

			void ReadV1()
			{
				uint16_t texture_count = m_stream.read_unsigned_short();
				uint32_t commands_count = m_stream.read_unsigned_short();

				for (uint16_t i = 0; texture_count > i; i++)
				{
					textures.emplace_back(m_stream.read_unsigned_short(), m_stream.read_unsigned_short());
				}

				for (uint32_t i = 0; commands_count > i; i++)
				{
					SpriteInstance& instance = sprites.emplace_back();

					m_stream.read_string(instance.name);
					instance.texture_index = m_stream.read_unsigned_byte();
					uint16_t vertices_count = m_stream.read_unsigned_byte();
					instance.vertices.reserve(vertices_count);

					for (uint16_t v = 0; vertices_count > v; v++)
					{
						auto& vertex = instance.vertices.emplace_back();

						vertex.u = m_stream.read_unsigned_short();
						vertex.v = m_stream.read_unsigned_short();
					}
				}
			}
		};
	}
}