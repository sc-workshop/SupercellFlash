#include "MatrixBank.h"
#include "flash/objects/SupercellSWF.h"
#include "flash/objects/SupercellSWF2.h"

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

		void MatrixBank::load(SupercellSWF& swf, const SC2::DataStorage* storage, SC2::Precision scale_presicion, SC2::Precision translation_presicion)
		{
			auto matrix_banks_vector = storage->matrix_banks();
			// Return if empty
			if (!matrix_banks_vector) return;

			float scale_multiplier = SupercellSWF2CompileTable::get_precision_multiplier(scale_presicion);
			float translation_multiplier = SupercellSWF2CompileTable::get_precision_multiplier(translation_presicion);

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
	}
}