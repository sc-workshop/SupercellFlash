#pragma once

#include "flash/flash_tags.h"
#include "Matrix2D.h"
#include "ColorTransform.h"
#include "flash/types/SWFContainer.hpp"

#include "flash/SC2/DataStorage_generated.h"
#include "flash/SC2/FileDescriptor_generated.h"
#include "flash/SC2/ExternalMatrixBank_generated.h"

#include "core/io/stream.h"

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
			SWFVector<Matrix2D, uint32_t> matrices;
			SWFVector<ColorTransform, uint32_t> color_transforms;

		public:
			bool get_matrix_index(const Matrix2D& matrix, uint16_t& index) const;
			bool get_colorTransform_index(const ColorTransform& color, uint16_t& index) const;

			virtual uint8_t tag(SupercellSWF& swf) const;

		public:
			static void load(SupercellSWF& swf, const SC2::FileDescriptor* descritptor, const SC2::DataStorage* storage);
			static void load_external(SupercellSWF& swf, const SC2::FileDescriptor* descritptor, wk::Stream& data);
		};
	}
}
