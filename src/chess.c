#include "./chess.h"
#include <stddef.h>

#define OPPOSITE(x) (x == WHITE ? BLACK : WHITE)

void start_game(struct game *game, enum piece_color bottom_color) {
	game->state = PLAYING;
	game->turn = WHITE;
	game->white_pieces_captured[0] = NONE;
	game->black_pieces_captured[0] = NONE;
	for (uint8_t y = 0; y < BOARD_H; ++y) {
		for (uint8_t x = 0; x < BOARD_W; ++x) {
			game->board[x][y].type = NONE;
			game->board[x][y].color = y < 4 ? OPPOSITE(bottom_color) : bottom_color;
			if (y == 1 || y == 6) {
				game->board[x][y].type = PAWN;
			} else if (y == 0 || y == 7) {
				switch (x) {
					case 0:
					case 7:
						game->board[x][y].type = ROOK;
						break;
					case 1:
					case 6:
						game->board[x][y].type = KNIGHT;
						break;
					case 2:
					case 5:
						game->board[x][y].type = BISHOP;
						break;
					case 3:
						game->board[x][y].type = QUEEN;
						break;
					case 4:
						game->board[x][y].type = KING;
						break;
				}
			}
		}
	}
}

void add_captured(enum piece_type array[], enum piece_type piece) {
	size_t i;
	for (i = 0; array[i] != NONE; ++i) {}
	array[i] = piece;
}
