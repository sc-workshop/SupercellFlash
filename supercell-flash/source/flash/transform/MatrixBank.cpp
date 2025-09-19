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

		void MatrixBank::load_external(SupercellSWF& swf, const SC2::FileDescriptor* descritptor, wk::Stream& stream)
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
					printf("short matrix data size: %d\n", bank->short_matrix_data_size());
					printf("compressed matrix data size: %d\n", bank->compressed_matrix_data_size());
					printf("short matrix count: %d\n", bank->short_matrix_count());
					printf("float matrix count: %d\n\n", bank->float_matrix_count());
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
					
					for (size_t i = uncompressed_matrices_count; total_matrices_count > i; i += 16)
					{
						// TODO: implement matricies decompression
						
						// int block_index = i / 16;
						// bank_data.seek(bank->float_matrix_count() * 24 + block_index * 4);
						// int position = bank_data.read_unsigned_int();

						// unsigned short* compressed_matrix_data = (unsigned short*)bank_data.data();
						// // printf("bank_data.read_unsigned_int(): %d\n", position);
						// int v16 = position >> 13;
						// unsigned short* v17 = &compressed_matrix_data[(position & 0x1FFF) * 6];
						// unsigned short v18 = *v17;
						// unsigned short v19 = v17[1];
						// unsigned short v20 = v17[2];
						// unsigned short v21 = v17[3];
						// unsigned short v22 = v17[4];
						// unsigned short v23 = v17[5];
						// // unsigned short* v24 = a2 + 6;
						// for (int sub_index = 0; sub_index < 16 && i + sub_index < total_matrices_count; sub_index++) {
						// 	unsigned int v26 = compressed_matrix_data[v16];
						// 	if ((v26 & 3) != 0)
						// 	{
						// 		unsigned int result = compressed_matrix_data[v16 + 1];
						// 		int v27 = v16 + 2;
						// 		unsigned int v28 = compressed_matrix_data[v16 + 2];
						// 		unsigned int v29 = v26 & 0xF;
						// 		if (v29 <= 4)
						// 		{
						// 			if (v29 == 1)
						// 			{
						// 				v22 += ((v26 << 14) | ((int)result << 30)) >> 18;
						// 				v23 += (int)result >> 2;
						// 				v16 = v16 + 2;
						// 			}
						// 			else
						// 			{
						// 				unsigned int v31 = v18 + (v26 << 21 >> 25);
						// 				unsigned int v32 = (v26 << 14) | ((int)result << 30);
						// 				unsigned int v33 = v18 + (v26 << 17 >> 21);
						// 				unsigned int v34 = v21 + (((v26 << 6) | ((int)result << 22)) >> 21);
						// 				unsigned int v35 = v22 + ((((int)result << 11) | (v28 << 27)) >> 21);
						// 				if (v29 == 3)
						// 					v18 = v33;
						// 				unsigned int v36 = v28 << 16;
						// 				if (v29 != 3)
						// 				{
						// 					v34 = v21;
						// 					v35 = v22;
						// 				}
						// 				unsigned int v37 = v23 + (v36 >> 21);
						// 				if (v29 == 3)
						// 					v16 = v16 + 3;
						// 				else
						// 					v37 = v23;
						// 				v21 += v32 >> 25;
						// 				if (v29 == 2)
						// 					v18 = v31;
						// 				v22 += (int)((int)result << 23) >> 25;
						// 				if (v29 != 2)
						// 				{
						// 					v21 = v34;
						// 					v22 = v35;
						// 				}
						// 				result = (unsigned int)((int)result << 16);
						// 				v23 += result >> 25;
						// 				if (v29 == 2)
						// 					v16 = v27;
						// 				else
						// 					v23 = v37;
						// 			}
						// 		}
						// 		else if ((v26 & 0xFu) > 6)
						// 		{
						// 			if (v29 == 7)
						// 			{
						// 				unsigned int v38 = compressed_matrix_data[v16 + 3];
						// 				unsigned int v39 = compressed_matrix_data[v16 + 4];
						// 				v18 += (int)v26 >> 4;
						// 				v19 += (int)((int)result << 20) >> 20;
						// 				v20 += (((int)result << 8) | (v28 << 24)) >> 20;
						// 				v21 += ((v28 << 12) | (v38 << 28)) >> 20;
						// 				v22 += ((v38 << 14) | (v39 << 30)) >> 18;
						// 				result = (v39 << 16);
						// 				v16 = v16 + 5;
						// 				v23 += v39 >> 2;
						// 			}
						// 			else if (v29 == 15)
						// 			{
						// 				v18 += result;
						// 				v19 += v28;
						// 				v20 += compressed_matrix_data[v16 + 3];
						// 				v21 += compressed_matrix_data[v16 + 4];
						// 				v22 += compressed_matrix_data[v16 + 5];
						// 				result = compressed_matrix_data[v16 + 6];
						// 				v23 += result;
						// 				v16 = v16 + 7;
						// 			}
						// 		}
						// 		else if (v29 == 5)
						// 		{
						// 			v18 += v26 << 21 >> 25;
						// 			v19 += ((v26 << 14) | ((int)result << 30)) >> 25;
						// 			v20 += (int)((int)result << 23) >> 25;
						// 			v21 += (short)result >> 9;
						// 			v22 += (char)v28;
						// 			result = (v28 << 16);
						// 			v16 = v16 + 3;
						// 			v23 += v28 >> 8;
						// 		}
						// 		else if (v29 == 6)
						// 		{
						// 			unsigned int v30 = compressed_matrix_data[v16 + 3];
						// 			v18 += v26 << 18 >> 22;
						// 			v19 += ((v26 << 8) | ((int)result << 24)) >> 22;
						// 			v20 += (((int)result << 14) | (v28 << 30)) >> 22;
						// 			v21 += v28 << 20 >> 22;
						// 			v22 += ((v28 << 10) | (v30 << 26)) >> 22;
						// 			result = (v30 << 16);
						// 			v16 = v16 + 4;
						// 			v23 += (short)v30 >> 6;
						// 		}
						// 	}
						// 	else
						// 	{
						// 		v22 += v26 << 23 >> 25;
						// 		// result = (v26 << 16);
						// 		v16 = v16 + 1;
						// 		v23 += (short)v26 >> 9;
						// 	}
						// 	// printf("decompressed matrix data: %.2f, %.2f, %.2f, %.2f, %.2f, %.2f\n", v18 / 1024.f, v19 / 1024.f, v20 / 1024.f, v21 / 1024.f, v22 / 20.f, v23 / 20.f);
						// 	auto& matrix = target.matrices[i + sub_index];

						// 	matrix.a = (short)v18 / 1024.f;
						// 	matrix.b = (short)v19 / 1024.f;
						// 	matrix.c = (short)v20 / 1024.f;
						// 	matrix.d = (short)v21 / 1024.f;
						// 	matrix.tx = (short)v22 / 20.f;
						// 	matrix.ty = (short)v23 / 20.f;
						// }
					}

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

				bank_data.seek(bank->float_matrix_count() * 24 + bank->compressed_matrix_data_size() * 4 + bank->short_matrix_data_size() * 2);

				// Color Transforms
				{
					target.color_transforms.resize(bank->color_transform_count());

					for (auto& color : target.color_transforms)
					{
						color.add.r = bank_data.read_unsigned_byte();
						color.add.g = bank_data.read_unsigned_byte();
						color.add.b = bank_data.read_unsigned_byte();

						color.alpha = bank_data.read_unsigned_byte();

						color.multiply.r = bank_data.read_unsigned_byte();
						color.multiply.g = bank_data.read_unsigned_byte();
						color.multiply.b = bank_data.read_unsigned_byte();
					}
				}

				bank_data.seek(bank->clip_data_offset());
				// Clip Data
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
