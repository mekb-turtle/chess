#ifndef CHESS_H
#define CHESS_H
#include <stdint.h>

#define BOARD_W 8
#define BOARD_H 8

enum piece_color {
	WHITE,
	BLACK
};

enum piece_type {
	NONE, KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN
};

struct piece {
	enum piece_type type;
	enum piece_color color;
};

struct position {
	uint8_t x, y;
};

enum game_state {
	MENU, PLAYING, PROMOTING, GAME_OVER
};

struct game {
	enum game_state state;
	enum piece_color turn;
	struct piece board[BOARD_W][BOARD_H];
	enum piece_type white_pieces_captured[BOARD_W*BOARD_H];
	enum piece_type black_pieces_captured[BOARD_W*BOARD_H];
};

void start_game(struct game *game, enum piece_color bottom_color);

void add_captured(enum piece_type array[], enum piece_type piece);
#endif //CHESS_H
