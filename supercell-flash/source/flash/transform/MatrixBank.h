#pragma once

#include "flash/flash_tags.h"
#include "Matrix2D.h"
#include "ColorTransform.h"
#include "flash/types/SWFContainer.hpp"

#include "flash/SC2/DataStorage_generated.h"
#include "flash/SC2/FileDescriptor_generated.h"

namespace sc
{
	namespace flash {
		class SupercellSWF;

		struct MatrixBank
		{
		public:
			MatrixBank(uint16_t matrix_count = 0, uint16_t color_transforms_count = 0);
			virtual ~MatrixBank() = default;
			MatrixBank(const MatrixBank&) = default;
			MatrixBank(MatrixBank&&) = default;
			MatrixBank& operator=(const MatrixBank&) = default;
			MatrixBank& operator=(MatrixBank&&) = default;

		public:
			SWFVector<Matrix2D> matrices;
			SWFVector<ColorTransform> color_transforms;

		public:
			bool get_matrix_index(const Matrix2D& matrix, uint16_t& index) const;
			bool get_colorTransform_index(const ColorTransform& color, uint16_t& index) const;

			virtual uint8_t tag(SupercellSWF& swf) const;

		public:
			static void load(SupercellSWF&, const SC2::DataStorage*, SC2::Precision scale_presicion, SC2::Precision translation_presicion);
		};
	}
}
