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
	bool started;
	bool flipped;
	move last_move;
	struct piece board[BOARD_W][BOARD_H];
	enum piece_type white_pieces_captured[20];
	enum piece_type black_pieces_captured[20];
	enum winner winner;
};

struct move_details {
	bool check;      // whether we are checking if the king is in check
	bool capture;    // capturing a piece
	bool en_passant; // https://chess.com/terms/en-passant
	bool castle;     // switching around the king and rook
	bool promotion;  // promoting a pawn to a queen, knight, bishop, or rook
};

bool pos_is_valid(pos position);
void start_game(struct game *game, bool flipped);
pos get_view_pos(struct game *game, pos position);
struct piece *get_piece(struct game *game, pos position);
bool is_check(struct game *game, enum piece_color color);
bool is_checkmate(struct game *game, enum piece_color color);
bool is_legal_move(struct game *game, move move, struct move_details *details);
bool move_piece_promote(struct game *game, move move, struct move_details *details, enum piece_type type);
bool move_piece(struct game *game, move move, struct move_details *details);
#endif //CHESS_H
