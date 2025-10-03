#include "MatrixBank.h"
#include "flash/objects/SupercellSWF.h"
#include "flash/objects/SupercellSWF2.h"

#include <execution>

#define floatEqual(a,b) (fabs(a - b) <= ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * 0.001f))

namespace sc {
	namespace flash {
		MatrixBank::MatrixBank(uint16_t matrix_count, uint16_t color_transforms_count)
		{
			matrices.resize(matrix_count);
			color_transforms.resize(color_transforms_count);
		}

		bool MatrixBank::get_matrix_index(const Matrix2D& matrix, uint16_t& index) const
		{
			if (floatEqual(matrix.a, 1.0f) &&
				floatEqual(matrix.b, 0.f) &&
				floatEqual(matrix.c, 0.f) &&
				floatEqual(matrix.d, 1.0f) &&
				floatEqual(matrix.tx, 0.f) &&
				floatEqual(matrix.ty, 0.f)) {
				index = 0xFFFF;
				return true;
			}

			auto result = std::find(std::execution::par_unseq, matrices.begin(), matrices.end(), matrix);
			if (result == matrices.end())
			{
				return false;
			}
			else
			{
				index = (uint16_t)std::distance(matrices.begin(), result);
				return true;
			}
		};

		bool MatrixBank::get_colorTransform_index(const ColorTransform& color, uint16_t& index) const
		{
			if (color.alpha == 0xFF &&
				color.add.r == 0 &&
				color.add.g == 0 &&
				color.add.b == 0 &&
				color.multiply.r == 0xFF &&
				color.multiply.g == 0xFF &&
				color.multiply.b == 0xFF) {
				index = 0xFFFF;
				return true;
			}

			auto result = std::find(std::execution::par_unseq, color_transforms.begin(), color_transforms.end(), color);
			if (result == color_transforms.end())
			{
				return false;
			}
			else
			{
				index = (uint16_t)std::distance(color_transforms.begin(), result);
				return true;
			}
		}

		uint8_t MatrixBank::tag(SupercellSWF&) const
		{
			return TAG_MATRIX_BANK;
		};

		void MatrixBank::load(SupercellSWF& swf, const SC2::FileDescriptor* descritptor, const SC2::DataStorage* storage)
		{
			auto matrix_banks_vector = storage->matrix_banks();
			// Return if empty
			if (!matrix_banks_vector) return;

			float scale_multiplier = SupercellSWF2CompileTable::get_precision_multiplier(descritptor->scale_precision());
			float translation_multiplier = SupercellSWF2CompileTable::get_precision_multiplier(descritptor->translation_precision());

			uint32_t matrix_bank_count = matrix_banks_vector->size();
			swf.matrixBanks.reserve((uint16_t)matrix_bank_count);

			for (auto data : *matrix_banks_vector)
			{
				MatrixBank& bank = swf.matrixBanks.emplace_back();

				auto matrices_vector = data->matrices();
				auto colors_vector = data->colors();
				auto half_matrices_vector = data->half_matrices();

				if (matrices_vector)
				{
					bank.matrices.reserve((uint16_t)matrices_vector->size());

					for (auto mdata : *matrices_vector)
					{
						Matrix2D& matrix = bank.matrices.emplace_back();
						matrix.a = mdata->a(); matrix.b = mdata->b(); matrix.c = mdata->c(); matrix.d = mdata->d();
						matrix.tx = mdata->tx(); matrix.ty = mdata->ty();
					}
					
				}
				else if (half_matrices_vector)
				{
					swf.sc2_compile_settings.use_half_precision_matrices = true;
					bank.matrices.reserve((uint16_t)half_matrices_vector->size());

					for (auto mdata : *half_matrices_vector)
					{
						Matrix2D& matrix = bank.matrices.emplace_back();
						matrix.a = (float)mdata->a() / scale_multiplier;
						matrix.b = (float)mdata->b() / scale_multiplier;
						matrix.c = (float)mdata->c() / scale_multiplier;
						matrix.d = (float)mdata->d() / scale_multiplier;
						matrix.tx = (float)mdata->tx() / translation_multiplier;
						matrix.ty = (float)mdata->ty() / translation_multiplier;
					}
				}

				if (colors_vector)
				{
					bank.color_transforms.reserve((uint16_t)colors_vector->size());

					for (auto cdata : *colors_vector)
					{
						ColorTransform& color = bank.color_transforms.emplace_back();
						color.add.r = cdata->r_add(); color.add.g = cdata->g_add(); color.add.b = cdata->b_add();
						color.multiply.r = cdata->r_mul(); color.multiply.g = cdata->g_mul(); color.multiply.b = cdata->b_mul();
						color.alpha = cdata->alpha();
					}
				}
			}
		}

		void MatrixBank::load_external(SupercellSWF& swf, const SC2::FileDescriptor*, wk::Stream& stream)
		{
			uint32_t descriptor_size = stream.read_unsigned_int();
			wk::SharedMemoryStream descriptor_data((uint8_t*)stream.data() + stream.position(), descriptor_size);
			const SC2::ExternalMatrixBanks* root = SC2::GetExternalMatrixBanks(descriptor_data.data());
			const auto* banks = root->banks();

			size_t banks_data_offset = stream.position() + descriptor_size;
			// float scale_multiplier = SupercellSWF2CompileTable::get_precision_multiplier(descritptor->scale_precision());
			// float translation_multiplier = SupercellSWF2CompileTable::get_precision_multiplier(descritptor->translation_precision());

			swf.matrixBanks.resize(banks->size());
			for (const auto* bank : *banks)
			{
				auto& target = swf.matrixBanks[bank->index()];
				wk::MemoryStream bank_data(bank->decompressed_data_size());

				{
					wk::MemoryStream compressed(bank->compressed_data_size());
					stream.seek(banks_data_offset + bank->compressed_data_offset());
					stream.read(compressed.data(), bank->compressed_data_size());
					compressed.seek(0);

					ZstdDecompressor decompressor;
					decompressor.decompress(compressed, bank_data);
					bank_data.seek(0);
				}

				// Matrices
				{
					size_t uncompressed_matrices_count = bank->short_matrix_count() + bank->float_matrix_count();
					if (uncompressed_matrices_count % 16 != 0)
						throw wk::Exception("uncompressed matrix count is not multiple of 16!");
					size_t total_matrices_count = std::min<size_t>(
						std::max<size_t>(
							uncompressed_matrices_count,
							bank->compressed_matrix_data_size() * 16),
						0xFFFF);

					uncompressed_matrices_count = std::min<size_t>(uncompressed_matrices_count, 0xFFFF);

					target.matrices.resize(total_matrices_count);
          
          // Float matrices
					for (size_t i = 0; bank->float_matrix_count() > i; i++)
					{
						auto& matrix = target.matrices[i];

						matrix.a = bank_data.read_float();
						matrix.b = bank_data.read_float();
						matrix.c = bank_data.read_float();
						matrix.d = bank_data.read_float();

						matrix.tx = bank_data.read_float();
						matrix.ty = bank_data.read_float();
					}
          
          // Compressed matrices
					// TODO: refactor this
					unsigned short* bank_data_ptr = (unsigned short*)bank_data.data();
					for (size_t i = uncompressed_matrices_count; total_matrices_count > i; i += 16)
					{
						int block_index = i >> 4;
						int position = *(int*)((unsigned char*)bank_data_ptr + bank->float_matrix_count() * 24 + block_index * 4);

						unsigned short* compressed_matrix_data = (unsigned short*)((unsigned char*)bank_data_ptr + bank->float_matrix_count() * 24 + bank->compressed_matrix_data_size() * 4);
						int v16 = position >> 13;
						unsigned short* v17 = &compressed_matrix_data[(position & 0x1FFF) * 6];
						unsigned int v18 = *v17;
						unsigned int v19 = v17[1];
						unsigned int v20 = v17[2];
						unsigned int v21 = v17[3];
						unsigned int v22 = v17[4];
						unsigned int v23 = v17[5];
						for (int sub_index = 0; sub_index < 16 && i + sub_index < total_matrices_count; sub_index++) {
							unsigned short v33 = compressed_matrix_data[v16];
							if ((v33 & 3) != 0)
							{
								unsigned short a3 = compressed_matrix_data[v16 + 1];
								unsigned short v34 = compressed_matrix_data[v16 + 2];
								unsigned short a1;
								unsigned short v35;
								switch (v33 & 0xF)
								{
								case 1u:
									v22 += (int32_t)((int64_t)((((uint64_t)v34 << 32) | ((uint64_t)a3 << 16) | (uint64_t)v33) << (64 - 14 - 4)) >> (64 - 14));
									v23 += (int32_t)((int64_t)((((uint64_t)v34 << 32) | ((uint64_t)a3 << 16) | (uint64_t)v33) << (64 - 14 - 18)) >> (64 - 14));
									v16 += 2;
									break;
								case 2u:
									v18 += (int32_t)((int64_t)((((uint64_t)v34 << 32) | ((uint64_t)a3 << 16) | (uint64_t)v33) << (64 - 7 - 4)) >> (64 - 7));
									v21 += (int32_t)((int64_t)((((uint64_t)v34 << 32) | ((uint64_t)a3 << 16) | (uint64_t)v33) << (64 - 7 - 11)) >> (64 - 7));
									v22 += (int32_t)((int64_t)((((uint64_t)v34 << 32) | ((uint64_t)a3 << 16) | (uint64_t)v33) << (64 - 7 - 18)) >> (64 - 7));
									v23 += (int32_t)((int64_t)((((uint64_t)v34 << 32) | ((uint64_t)a3 << 16) | (uint64_t)v33) << (64 - 7 - 25)) >> (64 - 7));
									v16 += 2;
									break;
								case 3u:
									v18 += (int32_t)((int64_t)((((uint64_t)v34 << 32) | ((uint64_t)a3 << 16) | (uint64_t)v33) << (64 - 11 - 4)) >> (64 - 11));
									v21 += (int32_t)((int64_t)((((uint64_t)v34 << 32) | ((uint64_t)a3 << 16) | (uint64_t)v33) << (64 - 11 - 15)) >> (64 - 11));
									v22 += (int32_t)((int64_t)((((uint64_t)v34 << 32) | ((uint64_t)a3 << 16) | (uint64_t)v33) << (64 - 11 - 26)) >> (64 - 11));
									v23 += (int32_t)((int64_t)((((uint64_t)v34 << 32) | ((uint64_t)a3 << 16) | (uint64_t)v33) << (64 - 11 - 37)) >> (64 - 11));
									v16 += 3;
									break;
								case 5u:
									v18 += (int32_t)((int64_t)((((uint64_t)v34 << 32) | ((uint64_t)a3 << 16) | (uint64_t)v33) << (64 - 7 - 4)) >> (64 - 7));
									v19 += (int32_t)((int64_t)((((uint64_t)v34 << 32) | ((uint64_t)a3 << 16) | (uint64_t)v33) << (64 - 7 - 11)) >> (64 - 7));
									v20 += (int32_t)((int64_t)((((uint64_t)v34 << 32) | ((uint64_t)a3 << 16) | (uint64_t)v33) << (64 - 7 - 18)) >> (64 - 7));
									v21 += (int32_t)((int64_t)((((uint64_t)v34 << 32) | ((uint64_t)a3 << 16) | (uint64_t)v33) << (64 - 7 - 25)) >> (64 - 7));
									v22 += (int32_t)((int64_t)((((uint64_t)v34 << 32) | ((uint64_t)a3 << 16) | (uint64_t)v33) << (64 - 8 - 32)) >> (64 - 8));
									v23 += (int32_t)((int64_t)((((uint64_t)v34 << 32) | ((uint64_t)a3 << 16) | (uint64_t)v33) << (64 - 8 - 40)) >> (64 - 8));
									v16 += 3;
									break;
								case 6u:
									a1 = compressed_matrix_data[v16 + 3];
									v18 += (int32_t)((int64_t)((((uint64_t)v34 << 32) | ((uint64_t)a3 << 16) | (uint64_t)v33) << (64 - 10 - 4)) >> (64 - 10));
									v19 += (int32_t)((int64_t)((((uint64_t)v34 << 32) | ((uint64_t)a3 << 16) | (uint64_t)v33) << (64 - 10 - 14)) >> (64 - 10));
									v20 += (int32_t)((int64_t)((((uint64_t)v34 << 32) | ((uint64_t)a3 << 16) | (uint64_t)v33) << (64 - 10 - 24)) >> (64 - 10));
									v21 += (int32_t)((int64_t)((((uint64_t)v34 << 32) | ((uint64_t)a3 << 16) | (uint64_t)v33) << (64 - 10 - 34)) >> (64 - 10));
									v22 += (int32_t)((int64_t)((((uint64_t)a1 << 32) | ((uint64_t)v34 << 16) | (uint64_t)a3) << (64 - 10 - 28)) >> (64 - 10));
									v23 += (int32_t)((int64_t)((((uint64_t)a1 << 32) | ((uint64_t)v34 << 16) | (uint64_t)a3) << (64 - 10 - 38)) >> (64 - 10));
									v16 += 4;
									break;
								case 7u:
									a1 = compressed_matrix_data[v16 + 3];
									v35 = compressed_matrix_data[v16 + 4];
									v18 += (int32_t)((int64_t)((((uint64_t)v34 << 32) | ((uint64_t)a3 << 16) | (uint64_t)v33) << (64 - 12 - 4)) >> (64 - 12));
									v19 += (int32_t)((int64_t)((((uint64_t)v34 << 32) | ((uint64_t)a3 << 16) | (uint64_t)v33) << (64 - 12 - 16)) >> (64 - 12));
									v20 += (int32_t)((int64_t)((((uint64_t)v34 << 32) | ((uint64_t)a3 << 16) | (uint64_t)v33) << (64 - 12 - 28)) >> (64 - 12));
									v21 += (int32_t)((int64_t)((((uint64_t)v35 << 32) | ((uint64_t)a1 << 16) | (uint64_t)v34) << (64 - 12 - 8)) >> (64 - 12));
									v22 += (int32_t)((int64_t)((((uint64_t)v35 << 32) | ((uint64_t)a1 << 16) | (uint64_t)v34) << (64 - 14 - 20)) >> (64 - 14));
									v23 += (int32_t)((int64_t)((((uint64_t)v35 << 32) | ((uint64_t)a1 << 16) | (uint64_t)v34) << (64 - 14 - 34)) >> (64 - 14));
									v16 += 5;
									break;
								case 0xFu:
									v18 += a3;
									v19 += v34;
									v20 += compressed_matrix_data[v16 + 3];
									v21 += compressed_matrix_data[v16 + 4];
									v22 += compressed_matrix_data[v16 + 5];
									v23 += compressed_matrix_data[v16 + 6];
									v16 += 7;
									break;
								default:
									break;
								}
							}
							else
							{
								v22 += (int32_t)((int64_t)((((uint64_t)0 << 32) | ((uint64_t)0 << 16) | (uint64_t)v33) << (64 - 7 - 2)) >> (64 - 7));
								v23 += (int32_t)((int64_t)((((uint64_t)0 << 32) | ((uint64_t)0 << 16) | (uint64_t)v33) << (64 - 7 - 9)) >> (64 - 7));
								++v16;
							}
							auto& matrix = target.matrices[i + sub_index];

							matrix.a = (int16_t)v18 / 1024.f;
							matrix.b = (int16_t)v19 / 1024.f;
							matrix.c = (int16_t)v20 / 1024.f;
							matrix.d = (int16_t)v21 / 1024.f;
							matrix.tx = (int16_t)v22 / 20.f;
							matrix.ty = (int16_t)v23 / 20.f;
						}
					}
          
          // Short matrices
					bank_data.seek(bank->float_matrix_count() * 24 + bank->compressed_matrix_data_size() * 4);
					for (size_t i = bank->float_matrix_count(); uncompressed_matrices_count > i; i++)
					{
						auto& matrix = target.matrices[i];

						matrix.a = (float)bank_data.read_short() / 1024.f;
						matrix.b = (float)bank_data.read_short() / 1024.f;
						matrix.c = (float)bank_data.read_short() / 1024.f;
						matrix.d = (float)bank_data.read_short() / 1024.f;
						matrix.tx = (float)bank_data.read_short() / 20.f;
						matrix.ty = (float)bank_data.read_short() / 20.f;
					}
				}
        
        // Color Transforms
				bank_data.seek(bank->float_matrix_count() * 24 + bank->compressed_matrix_data_size() * 4 + bank->short_matrix_data_size() * 2);
				{
					target.color_transforms.resize(bank->color_transform_count());

					for (auto& color : target.color_transforms)
					{
						color.multiply.r = bank_data.read_unsigned_byte();
						color.multiply.g = bank_data.read_unsigned_byte();
						color.multiply.b = bank_data.read_unsigned_byte();

						color.alpha = bank_data.read_unsigned_byte();

						color.add.r = bank_data.read_unsigned_byte();
						color.add.g = bank_data.read_unsigned_byte();
						color.add.b = bank_data.read_unsigned_byte();
					}
				}
        
        // Compressed MovieClip Data
				bank_data.seek(bank->clip_data_offset());
				{
					if (bank->clip_data_size() > 0)
					{
						target.compressed_clip_size = bank->clip_data_size();
						target.compressed_clip_data = new unsigned char[bank->clip_data_size()];
						bank_data.read(target.compressed_clip_data, bank->clip_data_size());
					}
				}
			}
		}
	}
}
