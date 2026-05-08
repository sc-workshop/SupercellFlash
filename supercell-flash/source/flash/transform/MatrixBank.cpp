#include "MatrixBank.h"

#include "core/algorithm/find.hpp"
#include "flash/objects/SupercellSWF.h"
#include "flash/objects/SupercellSWF2.h"

#include <execution>

#define floatEqual(a, b) (fabs(a - b) <= ((fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * 0.001f))

namespace sc::flash {
    static WK_INLINE int32_t extract_bits(uint64_t packed, int bit_count, int shift) {
        return (int32_t) ((int64_t) (packed << (64 - bit_count - shift)) >> (64 - bit_count));
    }

    static WK_INLINE uint64_t pack_bits(uint16_t low, uint16_t mid, uint16_t high) {
        return ((uint64_t) high << 32) | ((uint64_t) mid << 16) | (uint64_t) low;
    }

    MatrixBank::MatrixBank(uint16_t matrix_count, uint16_t color_transforms_count) {
        matrices.resize(matrix_count);
        color_transforms.resize(color_transforms_count);
    }

    bool MatrixBank::get_matrix_index(const Matrix2D& matrix, uint16_t& index) const {
        if (floatEqual(matrix.a, 1.0f) && floatEqual(matrix.b, 0.f) && floatEqual(matrix.c, 0.f) && floatEqual(matrix.d, 1.0f) && floatEqual(matrix.tx, 0.f) &&
            floatEqual(matrix.ty, 0.f)) {
            index = 0xFFFF;
            return true;
        }

        auto result = wk::find_parallel(matrices.begin(), matrices.end(), matrix);
        if (result == matrices.end()) {
            return false;
        } else {
            index = (uint16_t) std::distance(matrices.begin(), result);
            return true;
        }
    };

    bool MatrixBank::get_colorTransform_index(const ColorTransform& color, uint16_t& index) const {
        if (color.alpha == 0xFF && color.add.r == 0 && color.add.g == 0 && color.add.b == 0 && color.multiply.r == 0xFF && color.multiply.g == 0xFF &&
            color.multiply.b == 0xFF) {
            index = 0xFFFF;
            return true;
        }

        auto result = wk::find_parallel(color_transforms.begin(), color_transforms.end(), color);
        if (result == color_transforms.end()) {
            return false;
        } else {
            index = (uint16_t) std::distance(color_transforms.begin(), result);
            return true;
        }
    }

    uint8_t MatrixBank::tag(SupercellSWF&) const {
        return TAG_MATRIX_BANK;
    };

    void MatrixBank::load(SupercellSWF& swf, const SC2::Header* descritptor, const SC2::DataStorage* storage) {
        auto matrix_banks_vector = storage->matrix_banks();
        // Return if empty
        if (!matrix_banks_vector)
            return;

        float scale_multiplier = SupercellSWF2CompileTable::get_precision_multiplier(descritptor->scale_precision());
        float translation_multiplier = SupercellSWF2CompileTable::get_precision_multiplier(descritptor->translation_precision());

        uint32_t matrix_bank_count = matrix_banks_vector->size();
        swf.matrixBanks.reserve((uint16_t) matrix_bank_count);

        for (auto data : *matrix_banks_vector) {
            MatrixBank& bank = swf.matrixBanks.emplace_back();

            auto matrices_vector = data->matrices();
            auto colors_vector = data->colors();
            auto half_matrices_vector = data->half_matrices();

            if (matrices_vector) {
                bank.matrices.reserve((uint16_t) matrices_vector->size());

                for (auto mdata : *matrices_vector) {
                    Matrix2D& matrix = bank.matrices.emplace_back();
                    matrix.a = mdata->a();
                    matrix.b = mdata->b();
                    matrix.c = mdata->c();
                    matrix.d = mdata->d();
                    matrix.tx = mdata->tx();
                    matrix.ty = mdata->ty();
                }

            } else if (half_matrices_vector) {
                swf.sc2_compile_settings.use_half_precision_matrices = true;
                bank.matrices.reserve((uint16_t) half_matrices_vector->size());

                for (auto mdata : *half_matrices_vector) {
                    Matrix2D& matrix = bank.matrices.emplace_back();
                    matrix.a = (float) mdata->a() / scale_multiplier;
                    matrix.b = (float) mdata->b() / scale_multiplier;
                    matrix.c = (float) mdata->c() / scale_multiplier;
                    matrix.d = (float) mdata->d() / scale_multiplier;
                    matrix.tx = (float) mdata->tx() / translation_multiplier;
                    matrix.ty = (float) mdata->ty() / translation_multiplier;
                }
            }

            if (colors_vector) {
                bank.color_transforms.reserve((uint16_t) colors_vector->size());

                for (auto cdata : *colors_vector) {
                    ColorTransform& color = bank.color_transforms.emplace_back();
                    color.add.r = cdata->r_add();
                    color.add.g = cdata->g_add();
                    color.add.b = cdata->b_add();
                    color.multiply.r = cdata->r_mul();
                    color.multiply.g = cdata->g_mul();
                    color.multiply.b = cdata->b_mul();
                    color.alpha = cdata->alpha();
                }
            }
        }
    }

    void MatrixBank::load_external(SupercellSWF& swf, const SC2::Header*, wk::Stream& stream) {
        uint32_t descriptor_size = stream.read_unsigned_int();
        wk::SharedMemoryStream descriptor_data((uint8_t*) stream.data() + stream.position(), descriptor_size);
        const SC2::ExternalMatrixBanks* root = SC2::GetExternalMatrixBanks(descriptor_data.data());
        const auto* banks = root->banks();

        size_t banks_data_offset = stream.position() + descriptor_size;
        // float scale_multiplier = SupercellSWF2CompileTable::get_precision_multiplier(descritptor->scale_precision());
        // float translation_multiplier = SupercellSWF2CompileTable::get_precision_multiplier(descritptor->translation_precision());

        swf.matrixBanks.resize(banks->size());
        for (const auto* bank : *banks) {
            auto& target = swf.matrixBanks[bank->index()];
            wk::MemoryStream bank_buffer(bank->decompressed_data_size());

            {
                wk::MemoryStream compressed(bank->compressed_data_size());
                stream.seek(banks_data_offset + bank->compressed_data_offset());
                stream.read(compressed.data(), bank->compressed_data_size());
                compressed.seek(0);

                ZstdDecompressor decompressor;
                decompressor.decompress(compressed, bank_buffer);
                bank_buffer.seek(0);
            }

            // Matrices
            {
                uint32_t uncompressed_matrices_count = bank->short_matrix_count() + bank->float_matrix_count();
                if (uncompressed_matrices_count % 16 != 0)
                    throw wk::Exception("uncompressed delta_matrix count is not multiple of 16!");
                uint16_t total_matrices_count =
                    (uint16_t) std::min<uint32_t>(std::max<uint32_t>(uncompressed_matrices_count, bank->compressed_matrix_data_size() * 16), 0xFFFF);

                uncompressed_matrices_count = std::min<uint32_t>(uncompressed_matrices_count, 0xFFFF);

                target.matrices.resize(total_matrices_count);

                // Float matrices
                for (uint16_t i = 0; (uint16_t) bank->float_matrix_count() > i; i++) {
                    auto& matrix = target.matrices[i];

                    matrix.a = bank_buffer.read_float();
                    matrix.b = bank_buffer.read_float();
                    matrix.c = bank_buffer.read_float();
                    matrix.d = bank_buffer.read_float();

                    matrix.tx = bank_buffer.read_float();
                    matrix.ty = bank_buffer.read_float();
                }

                // Compressed matrices
                size_t float_matrices_size = static_cast<size_t>(bank->float_matrix_count()) * 24;
                size_t compressed_matrices_size = static_cast<size_t>(bank->compressed_matrix_data_size()) * 4;
                size_t compressed_matrix_data_offset = float_matrices_size + compressed_matrices_size;
                auto* compressed_matrix_data = reinterpret_cast<uint16_t*>(reinterpret_cast<uint8_t*>(bank_buffer.data()) + compressed_matrix_data_offset);

                for (uint32_t i = uncompressed_matrices_count; total_matrices_count > i; i += 16) {
                    size_t block_index = i >> 4;
                    int32_t encoded_block = *reinterpret_cast<int32_t*>(reinterpret_cast<uint8_t*>(bank_buffer.data()) + float_matrices_size + block_index * 4);

                    int32_t data_offset = encoded_block >> 13;
                    uint16_t base_matrix_index = encoded_block & 0x1FFF;
                    uint16_t* base_matrix = &compressed_matrix_data[base_matrix_index * 6];

                    uint32_t a = base_matrix[0];
                    uint32_t b = base_matrix[1];
                    uint32_t c = base_matrix[2];
                    uint32_t d = base_matrix[3];
                    uint32_t tx = base_matrix[4];
                    uint32_t ty = base_matrix[5];

                    for (int sub_index = 0; sub_index < 16 && i + sub_index < total_matrices_count; sub_index++) {
                        uint16_t flags = compressed_matrix_data[data_offset];
                        if ((flags & 3) != 0) {
                            uint16_t low_bits = compressed_matrix_data[data_offset + 1];
                            uint16_t high_bits = compressed_matrix_data[data_offset + 2];
                            uint64_t packed = pack_bits(flags, low_bits, high_bits);

                            switch (flags & 0xF) {
                                // Translation (High precision: 14 bits for each value)
                                case 1u:
                                    tx += extract_bits(packed, 14, 4);
                                    ty += extract_bits(packed, 14, 18);
                                    data_offset += 2;
                                    break;
                                // Translation + Scale (Low precision: 7 bits for each value)
                                case 2u:
                                    a += extract_bits(packed, 7, 4);
                                    d += extract_bits(packed, 7, 11);
                                    tx += extract_bits(packed, 7, 18);
                                    ty += extract_bits(packed, 7, 25);
                                    data_offset += 2;
                                    break;
                                // Translation + Scale (Mid precision: 11 bits for each value)
                                case 3u:
                                    a += extract_bits(packed, 11, 4);
                                    d += extract_bits(packed, 11, 15);
                                    tx += extract_bits(packed, 11, 26);
                                    ty += extract_bits(packed, 11, 37);
                                    data_offset += 3;
                                    break;
                                // Translation + Scale + Skew (Low precision: 7 bits for scale and skew, 8 bits for translation)
                                case 5u:
                                    a += extract_bits(packed, 7, 4);
                                    b += extract_bits(packed, 7, 11);
                                    c += extract_bits(packed, 7, 18);
                                    d += extract_bits(packed, 7, 25);
                                    tx += extract_bits(packed, 8, 32);
                                    ty += extract_bits(packed, 8, 40);
                                    data_offset += 3;
                                    break;
                                // Translation + Scale + Skew (Mid precision: 10 bits for each value)
                                case 6u: {
                                    uint16_t mid_bits = compressed_matrix_data[data_offset + 3];
                                    uint64_t translation_bits = pack_bits(low_bits, high_bits, mid_bits);

                                    a += extract_bits(packed, 10, 4);
                                    b += extract_bits(packed, 10, 14);
                                    c += extract_bits(packed, 10, 24);
                                    d += extract_bits(packed, 10, 34);
                                    tx += extract_bits(translation_bits, 10, 28);
                                    ty += extract_bits(translation_bits, 10, 38);
                                    data_offset += 4;
                                } break;
                                // Translation + Scale + Skew (High precision: 12 bits for scale and skew, 14 bits for translation)
                                case 7u: {
                                    uint16_t mid_bits = compressed_matrix_data[data_offset + 3];
                                    uint16_t highest_bits = compressed_matrix_data[data_offset + 4];
                                    uint64_t translation_bits = pack_bits(high_bits, mid_bits, highest_bits);

                                    a += extract_bits(packed, 12, 4);
                                    b += extract_bits(packed, 12, 16);
                                    c += extract_bits(packed, 12, 28);
                                    d += extract_bits(translation_bits, 12, 8);
                                    tx += extract_bits(translation_bits, 14, 20);
                                    ty += extract_bits(translation_bits, 14, 34);
                                    data_offset += 5;
                                } break;
                                // Translation + Scale + Skew (Full Precision)
                                case 0xFu:
                                    a += low_bits;
                                    b += high_bits;
                                    c += compressed_matrix_data[data_offset + 3];
                                    d += compressed_matrix_data[data_offset + 4];
                                    tx += compressed_matrix_data[data_offset + 5];
                                    ty += compressed_matrix_data[data_offset + 6];
                                    data_offset += 7;
                                    break;
                                default:
                                    throw wk::Exception("Unknown compressed matrix encoding");
                            }
                        } else {
                            tx += extract_bits(flags, 7, 2);
                            ty += extract_bits(flags, 7, 9);
                            data_offset++;
                        }

                        auto& matrix = target.matrices[static_cast<uint16_t>(i + sub_index)];
                        matrix.a = (int16_t) a / 1024.f;
                        matrix.b = (int16_t) b / 1024.f;
                        matrix.c = (int16_t) c / 1024.f;
                        matrix.d = (int16_t) d / 1024.f;
                        matrix.tx = (int16_t) tx / 20.f;
                        matrix.ty = (int16_t) ty / 20.f;
                    }
                }

                // Short matrices
                bank_buffer.seek(compressed_matrix_data_offset);
                for (uint16_t i = (uint16_t) bank->float_matrix_count(); uncompressed_matrices_count > i; i++) {
                    auto& matrix = target.matrices[i];

                    matrix.a = (float) bank_buffer.read_short() / 1024.f;
                    matrix.b = (float) bank_buffer.read_short() / 1024.f;
                    matrix.c = (float) bank_buffer.read_short() / 1024.f;
                    matrix.d = (float) bank_buffer.read_short() / 1024.f;
                    matrix.tx = (float) bank_buffer.read_short() / 20.f;
                    matrix.ty = (float) bank_buffer.read_short() / 20.f;
                }
            }

            // Color Transforms
            bank_buffer.seek(bank->float_matrix_count() * 24 + bank->compressed_matrix_data_size() * 4 + bank->short_matrix_data_size() * 2);
            {
                target.color_transforms.resize((uint16_t) bank->color_transform_count());

                for (auto& color : target.color_transforms) {
                    color.multiply.r = bank_buffer.read_unsigned_byte();
                    color.multiply.g = bank_buffer.read_unsigned_byte();
                    color.multiply.b = bank_buffer.read_unsigned_byte();

                    color.alpha = bank_buffer.read_unsigned_byte();

                    color.add.r = bank_buffer.read_unsigned_byte();
                    color.add.g = bank_buffer.read_unsigned_byte();
                    color.add.b = bank_buffer.read_unsigned_byte();
                }
            }

            // Compressed MovieClip Data
            bank_buffer.seek(bank->clip_data_offset());
            {
                if (bank->clip_data_size() > 0) {
                    target.movieclip_elements = SWFVector<uint8_t, uint32_t>(bank->clip_data_size());
                    bank_buffer.read(target.movieclip_elements.data(), bank->clip_data_size());
                }
            }
        }
    }
}
