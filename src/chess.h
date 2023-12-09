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
	STALEMATE,
	INSUFFICIENT_MATERIAL,
	DRAW,
	UNKNOWN,
};

enum piece_type {
	NONE,
	PAWN,
	KNIGHT,
	BISHOP,
	ROOK,
	QUEEN,
	KING,
	NUM_PIECES
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
	enum piece_type white_pieces_captured[32];
	enum piece_type black_pieces_captured[32];
	enum winner winner;
};

struct move_details {
	bool check;      // whether we are checking if the king is in check
	bool capture;    // capturing a piece
	bool en_passant; // https://chess.com/terms/en-passant
	bool castle;     // https://chess.com/terms/castling-chess
	bool promotion;  // promoting a pawn to a queen, knight, bishop, or rook
};

void start_game(struct game *game, bool flipped);
bool pos_is_valid(pos position);
pos get_view_pos(struct game *game, pos position);
struct piece *get_piece(struct game *game, pos position);
bool is_check(struct game *game, enum piece_color color);
bool is_checkmate(struct game *game, enum piece_color color);
bool is_stalemate(struct game *game, enum piece_color color);
bool is_insufficient(struct game *game, enum piece_color color);
bool is_legal_move(struct game *game, move move, struct move_details *details);
bool move_piece_promote(struct game *game, move move, struct move_details *details, enum piece_type type);
bool move_piece(struct game *game, move move, struct move_details *details);
#endif //CHESS_H
