#include "./styling.h"

char *get_piece_text(enum piece_type type, enum piece_color color) {
	if (color == BLACK) {
		switch (type) {
			case KING:
				return TEXT_BLACK_KING;
			case QUEEN:
				return TEXT_BLACK_QUEEN;
			case ROOK:
				return TEXT_BLACK_ROOK;
			case BISHOP:
				return TEXT_BLACK_BISHOP;
			case KNIGHT:
				return TEXT_BLACK_KNIGHT;
			case PAWN:
				return TEXT_BLACK_PAWN;
			default:
		}
	} else if (color == WHITE) {
		switch (type) {
			case KING:
				return TEXT_WHITE_KING;
			case QUEEN:
				return TEXT_WHITE_QUEEN;
			case ROOK:
				return TEXT_WHITE_ROOK;
			case BISHOP:
				return TEXT_WHITE_BISHOP;
			case KNIGHT:
				return TEXT_WHITE_KNIGHT;
			case PAWN:
				return TEXT_WHITE_PAWN;
			default:
		}
	}
	return "";
}

nk_bool draw_piece(struct nk_context *ctx, enum piece_type type, enum piece_color color, bool is_board) {
	char *label = get_piece_text(type, color);
	if (is_board) {
		return nk_button_label(ctx, label);
	} else {
		nk_label(ctx, label, NK_TEXT_CENTERED);
	}
	return false;
}
