#pragma once
#include <stdint.h>

namespace sc
{
	namespace flash {
		constexpr uint8_t TAG_END = 0;

		constexpr uint8_t TAG_TEXTURE = 1;
		constexpr uint8_t TAG_SHAPE = 2;
		constexpr uint8_t TAG_MOVIE_CLIP = 3; // deprecated
		constexpr uint8_t TAG_SHAPE_DRAW_BITMAP_COMMAND = 4;
		constexpr uint8_t TAG_MOVIE_CLIP_FRAME = 5; // deprecated
		constexpr uint8_t TAG_SHAPE_DRAW_COLOR_FILL_COMMAND = 6; // deprecated
		constexpr uint8_t TAG_TEXT_FIELD = 7;

		constexpr uint8_t TAG_MATRIX_2x3 = 8;
		constexpr uint8_t TAG_COLOR_TRANSFORM = 9;

		constexpr uint8_t TAG_MOVIE_CLIP_2 = 10;
		constexpr uint8_t TAG_MOVIE_CLIP_FRAME_2 = 11;
		constexpr uint8_t TAG_MOVIE_CLIP_3 = 12;
		constexpr uint8_t TAG_TIMELINE_INDEXES = 13; // deprecated
		constexpr uint8_t TAG_MOVIE_CLIP_4 = 14; // deprecated

		constexpr uint8_t TAG_TEXT_FIELD_2 = 15;
		constexpr uint8_t TAG_TEXTURE_2 = 16;

		constexpr uint8_t TAG_SHAPE_DRAW_BITMAP_COMMAND_2 = 17;
		constexpr uint8_t TAG_SHAPE_2 = 18;

		constexpr uint8_t TAG_TEXTURE_3 = 19;
		constexpr uint8_t TAG_TEXT_FIELD_3 = 20;
		constexpr uint8_t TAG_TEXT_FIELD_4 = 21;
		constexpr uint8_t TAG_SHAPE_DRAW_BITMAP_COMMAND_3 = 22;
		constexpr uint8_t TAG_DISABLE_LOW_RES_TEXTURE = 23;
		constexpr uint8_t TAG_TEXTURE_4 = 24;
		constexpr uint8_t TAG_TEXT_FIELD_5 = 25;
		constexpr uint8_t TAG_USE_EXTERNAL_TEXTURE = 26;
		constexpr uint8_t TAG_TEXTURE_5 = 27;
		constexpr uint8_t TAG_TEXTURE_6 = 28;
		constexpr uint8_t TAG_TEXTURE_7 = 29;
		constexpr uint8_t TAG_USE_MULTI_RES_TEXTURE = 30;
		constexpr uint8_t TAG_SCALING_GRID = 31;
		constexpr uint8_t TAG_TEXTURE_FILE_SUFFIXES = 32;
		constexpr uint8_t TAG_TEXT_FIELD_6 = 33;
		constexpr uint8_t TAG_TEXTURE_8 = 34;
		constexpr uint8_t TAG_MOVIE_CLIP_5 = 35;

		constexpr uint8_t TAG_MATRIX_2x3_2 = 36;

		constexpr uint8_t TAG_MOVIE_CLIP_MODIFIERS_COUNT = 37;
		constexpr uint8_t TAG_MOVIE_CLIP_MODIFIER = 38; // Mask layer
		constexpr uint8_t TAG_MOVIE_CLIP_MODIFIER_2 = 39; // Masked layers start
		constexpr uint8_t TAG_MOVIE_CLIP_MODIFIER_3 = 40; // Masked layers end

		constexpr uint8_t TAG_MATRIX_BANK_INDEX = 41; // Movieclip frame tag
		constexpr uint8_t TAG_MATRIX_BANK = 42;

		constexpr uint8_t TAG_TEXT_FIELD_7 = 43;
		constexpr uint8_t TAG_TEXT_FIELD_8 = 44;

		constexpr uint8_t TAG_TEXTURE_9 = 45; // SWFTexture Khronos Texture
		constexpr uint8_t TAG_TEXT_FIELD_9 = 46; // Typograph

		constexpr uint8_t TAG_TEXTURE_10 = 47; // External Texture File
		constexpr uint8_t TAG_STREAMING_TEXTURE_ID_1 = 48; // Some Kind Of Flag
		constexpr uint8_t TAG_MOVIE_CLIP_6 = 49; // Custom Property Index
	}
}