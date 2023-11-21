#ifndef UTIL_H
#define UTIL_H
#include <stddef.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include "./nuklear.h"
#include "./chess.h"

#define SET_BUTTON_THEME(button_style, theme_color, theme_color_hover, theme_color_active, type_prefix, type_suffix) \
	{                                                                                                                \
		button_style.type_prefix##normal type_suffix = chess_theme[CHESS_COLOR_##theme_color];                       \
		button_style.type_prefix##hover type_suffix = chess_theme[CHESS_COLOR_##theme_color_hover];                  \
		button_style.type_prefix##active type_suffix = chess_theme[CHESS_COLOR_##theme_color_active];                \
	}

#define SET_BUTTON_BG_THEME(button_style, theme_color) SET_BUTTON_THEME(button_style, theme_color, theme_color##_HOVER, theme_color##_ACTIVE, , .data.color)
#define SET_BUTTON_FG_THEME(button_style, theme_color) SET_BUTTON_THEME(button_style, theme_color, theme_color, theme_color, text_, )

enum chess_style {
	CHESS_COLOR_PIECE_BLACK,
	CHESS_COLOR_PIECE_WHITE,
	CHESS_COLOR_1,
	CHESS_COLOR_2,
	CHESS_COLOR_MOVE,
	CHESS_COLOR_MOVED,
	CHESS_COLOR_PIECE_BLACK_HOVER,
	CHESS_COLOR_PIECE_WHITE_HOVER,
	CHESS_COLOR_1_HOVER,
	CHESS_COLOR_2_HOVER,
	CHESS_COLOR_MOVE_HOVER,
	CHESS_COLOR_MOVED_HOVER,
	CHESS_COLOR_PIECE_BLACK_ACTIVE,
	CHESS_COLOR_PIECE_WHITE_ACTIVE,
	CHESS_COLOR_1_ACTIVE,
	CHESS_COLOR_2_ACTIVE,
	CHESS_COLOR_MOVE_ACTIVE,
	CHESS_COLOR_MOVED_ACTIVE,
	CHESS_COLOR_COUNT,
};

char *get_piece_text(struct piece piece) {
	switch (piece.type) {
		case KING:
			return piece.color == BLACK ? TEXT_BLACK_KING : TEXT_WHITE_KING;
		case QUEEN:
			return piece.color == BLACK ? TEXT_BLACK_QUEEN : TEXT_WHITE_QUEEN;
		case ROOK:
			return piece.color == BLACK ? TEXT_BLACK_ROOK : TEXT_WHITE_ROOK;
		case BISHOP:
			return piece.color == BLACK ? TEXT_BLACK_BISHOP : TEXT_WHITE_BISHOP;
		case KNIGHT:
			return piece.color == BLACK ? TEXT_BLACK_KNIGHT : TEXT_WHITE_KNIGHT;
		case PAWN:
			return piece.color == BLACK ? TEXT_BLACK_PAWN : TEXT_WHITE_PAWN;
		default:
			return "";
	}
}

nk_bool draw_piece(struct nk_context *ctx, struct piece piece, bool is_board) {
	char *label = get_piece_text(piece);
	if (is_board) {
		return nk_button_label(ctx, label);
	} else {
		nk_label(ctx, label, NK_TEXT_CENTERED);
	}
	return false;
}

#endif //UTIL_H
