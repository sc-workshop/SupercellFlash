#include "MatrixBank.h"
#include "flash/objects/SupercellSWF.h"

#include <execution>

#define floatEqual(a,b) (fabs(a - b) <= ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * 0.001))

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
				floatEqual(matrix.b, 0) &&
				floatEqual(matrix.c, 0) &&
				floatEqual(matrix.d, 1.0f) &&
				floatEqual(matrix.tx, 0) &&
				floatEqual(matrix.ty, 0)) {
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

		void MatrixBank::load(SupercellSWF& swf, const SC2::DataStorage* storage)
		{
			auto matrix_banks_vector = storage->matrix_banks();
			// Return if empty
			if (!matrix_banks_vector) return;

			uint32_t matrix_bank_count = matrix_banks_vector->size();
			swf.matrixBanks.reserve(matrix_bank_count);

			for (uint32_t i = 0; matrix_bank_count > i; i++)
			{
				auto bank_data = matrix_banks_vector->Get(i);
				MatrixBank& bank = swf.matrixBanks.emplace_back();

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
	}
}