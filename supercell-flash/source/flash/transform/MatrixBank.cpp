#include "MatrixBank.h"

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
				index = std::distance(matrices.begin(), result);
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
				index = std::distance(color_transforms.begin(), result);
				return true;
			}
		}

		uint8_t MatrixBank::tag(SupercellSWF&) const
		{
			return TAG_MATRIX_BANK;
		};
	}
}