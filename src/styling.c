#include "./styling.h"

char *get_piece_text(enum piece_type type, enum piece_color color) {
	switch (type) {
		case KING:
			return color == BLACK ? TEXT_BLACK_KING : TEXT_WHITE_KING;
		case QUEEN:
			return color == BLACK ? TEXT_BLACK_QUEEN : TEXT_WHITE_QUEEN;
		case ROOK:
			return color == BLACK ? TEXT_BLACK_ROOK : TEXT_WHITE_ROOK;
		case BISHOP:
			return color == BLACK ? TEXT_BLACK_BISHOP : TEXT_WHITE_BISHOP;
		case KNIGHT:
			return color == BLACK ? TEXT_BLACK_KNIGHT : TEXT_WHITE_KNIGHT;
		case PAWN:
			return color == BLACK ? TEXT_BLACK_PAWN : TEXT_WHITE_PAWN;
		default:
			return "";
	}
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
