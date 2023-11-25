#ifndef CHESS_H
#define CHESS_H
#include "./util.h"

#define BOARD_W 8
#define BOARD_H 8

enum piece_color {
	WHITE,
	BLACK
};

enum winner {
	NO_WINNER,
	WINNER_WHITE,
	WINNER_BLACK,
	UNKNOWN,
	STALEMATE,
};

enum piece_type {
	NONE,
	KING,
	QUEEN,
	ROOK,
	BISHOP,
	KNIGHT,
	PAWN
};

struct piece {
	enum piece_type type;
	enum piece_color color;
	uint8_t turns;
};

// TODO: move this to main.c
struct game {
	enum piece_color turn;
	enum piece_color bottom_color;
	pos last_moved;
	struct piece board[BOARD_W][BOARD_H];
	enum piece_type white_pieces_captured[20];
	enum piece_type black_pieces_captured[20];
	enum winner winner;
};

enum move_result {
	DISALLOWED,
	ALLOWED,
	PROMOTION
};

bool pos_is_valid(pos position);
void start_game(struct game *game, enum piece_color bottom_color);
void add_captured(enum piece_type array[], enum piece_type piece);
struct piece *get_piece(struct game *game, pos position);
bool is_check(struct game *game, enum piece_color color);
bool is_checkmate(struct game *game, enum piece_color color);
enum move_result is_legal_move(struct game *game, move move);
enum move_result move_piece_promote(struct game *game, move move, enum piece_type type);
enum move_result move_piece(struct game *game, move move);
#endif //CHESS_H
