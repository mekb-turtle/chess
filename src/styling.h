#ifndef STYLING_H
#define STYLING_H
#include <stddef.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include "./config.h"
#include "./nuklear.h"
#include "./chess.h"

#define SET_BUTTON_BG_THEME(button_style, theme_color)                                           \
	{                                                                                            \
		button_style.button.normal.data.color = chess_theme[CHESS_COLOR_##theme_color];          \
		button_style.button.hover.data.color = chess_theme[CHESS_COLOR_##theme_color##_HOVER];   \
		button_style.button.active.data.color = chess_theme[CHESS_COLOR_##theme_color##_ACTIVE]; \
	}
#define SET_BUTTON_FG_THEME(button_style, theme_color)                            \
	{                                                                             \
		button_style.button.text_normal = chess_theme[CHESS_COLOR_##theme_color]; \
		button_style.button.text_hover = chess_theme[CHESS_COLOR_##theme_color];  \
		button_style.button.text_active = chess_theme[CHESS_COLOR_##theme_color]; \
	}
#define SET_TEXT_FG_THEME(style, theme_color)                      \
	{                                                              \
		style.text.color = chess_theme[CHESS_COLOR_##theme_color]; \
	}

enum chess_style {
	CHESS_COLOR_PIECE_BLACK,
	CHESS_COLOR_PIECE_WHITE,
	CHESS_COLOR_1,
	CHESS_COLOR_2,
	CHESS_COLOR_MOVE,
	CHESS_COLOR_MOVE_AVAILABLE,
	CHESS_COLOR_MOVE_CAPTURE,
	CHESS_COLOR_MOVE_LAST,
	CHESS_COLOR_PIECE_BLACK_HOVER,
	CHESS_COLOR_PIECE_WHITE_HOVER,
	CHESS_COLOR_1_HOVER,
	CHESS_COLOR_2_HOVER,
	CHESS_COLOR_MOVE_HOVER,
	CHESS_COLOR_MOVE_AVAILABLE_HOVER,
	CHESS_COLOR_MOVE_CAPTURE_HOVER,
	CHESS_COLOR_MOVE_LAST_HOVER,
	CHESS_COLOR_PIECE_BLACK_ACTIVE,
	CHESS_COLOR_PIECE_WHITE_ACTIVE,
	CHESS_COLOR_1_ACTIVE,
	CHESS_COLOR_2_ACTIVE,
	CHESS_COLOR_MOVE_ACTIVE,
	CHESS_COLOR_MOVE_AVAILABLE_ACTIVE,
	CHESS_COLOR_MOVE_CAPTURE_ACTIVE,
	CHESS_COLOR_MOVE_LAST_ACTIVE,
	CHESS_COLOR_COUNT,
};

nk_bool draw_piece(struct nk_context *ctx, enum piece_type type, enum piece_color color, bool is_board);

#endif //STYLING_H
