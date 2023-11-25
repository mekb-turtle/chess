#ifndef STYLING_H
#define STYLING_H
#include <stddef.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include "./config.h"
#include "./nuklear.h"
#include "./chess.h"

// TODO: make this code not suck
#define LOAD_FONT_SYSTEM(font_name)                                  \
	{                                                                \
		char *font_file = get_font_file(font_name);                  \
		if (!font_file) {                                            \
			warnx("failed to get font: %s", font_name);              \
			goto exit;                                               \
		}                                                            \
                                                                     \
		FILE *font_fp = fopen(font_file, "rb");                      \
		if (!font_fp) {                                              \
			warnx("failed to open font file: %s", font_file);        \
			goto exit;                                               \
		}                                                            \
                                                                     \
		void *font_file_data;                                        \
		size_t font_file_size;                                       \
		if (!read_file(font_fp, &font_file_data, &font_file_size)) { \
			warnx("failed to read file: %s", font_file);             \
			fclose(font_fp);                                         \
			goto exit;                                               \
		}                                                            \
		fclose(font_fp);                                             \
		LOAD_FONT(font_file_data, font_file_size)                    \
		free(font_file_data);                                        \
	}

// TODO: make this code not suck
#define LOAD_FONT(font_file_data, font_file_size)                                          \
	{                                                                                      \
		for (size_t i = 0; i < 100; ++i) {                                                 \
			fonts[font_len][i] = load_font(font_file_data, font_file_size, i + 1, 1, ctx); \
		}                                                                                  \
		++font_len;                                                                        \
	}

#define SET_FONT(ctx, index, size)                                    \
	{                                                                 \
		int font_size = size;                                         \
		if (font_size < 1) font_size = 1;                             \
		if (font_size > 100) font_size = 100;                         \
		nk_style_set_font(ctx, &fonts[index][font_size - 1]->handle); \
	}

#define SET_BUTTON_BG_THEME(button_style, theme_color)                                    \
	{                                                                                     \
		button_style.normal.data.color = chess_theme[CHESS_COLOR_##theme_color];          \
		button_style.hover.data.color = chess_theme[CHESS_COLOR_##theme_color##_HOVER];   \
		button_style.active.data.color = chess_theme[CHESS_COLOR_##theme_color##_ACTIVE]; \
	}
#define SET_BUTTON_FG_THEME(button_style, theme_color)                     \
	{                                                                      \
		button_style.text_normal = chess_theme[CHESS_COLOR_##theme_color]; \
		button_style.text_hover = chess_theme[CHESS_COLOR_##theme_color];  \
		button_style.text_active = chess_theme[CHESS_COLOR_##theme_color]; \
	}

enum chess_style {
	CHESS_COLOR_PIECE_BLACK,
	CHESS_COLOR_PIECE_WHITE,
	CHESS_COLOR_1,
	CHESS_COLOR_2,
	CHESS_COLOR_MOVE1,
	CHESS_COLOR_MOVE2,
	CHESS_COLOR_PIECE_BLACK_HOVER,
	CHESS_COLOR_PIECE_WHITE_HOVER,
	CHESS_COLOR_1_HOVER,
	CHESS_COLOR_2_HOVER,
	CHESS_COLOR_MOVE1_HOVER,
	CHESS_COLOR_MOVE2_HOVER,
	CHESS_COLOR_PIECE_BLACK_ACTIVE,
	CHESS_COLOR_PIECE_WHITE_ACTIVE,
	CHESS_COLOR_1_ACTIVE,
	CHESS_COLOR_2_ACTIVE,
	CHESS_COLOR_MOVE1_ACTIVE,
	CHESS_COLOR_MOVE2_ACTIVE,
	CHESS_COLOR_COUNT,
};

nk_bool draw_piece(struct nk_context *ctx, enum piece_type type, enum piece_color color, bool is_board);

#endif //STYLING_H
