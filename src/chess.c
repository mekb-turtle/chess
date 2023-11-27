#include "./chess.h"
#include <stddef.h>

#define OPPOSITE(x) (x == WHITE ? BLACK : WHITE)

#define DETAILS(details)                \
	struct move_details details_ = {0}; \
	if (!details) details = &details_;

void start_game(struct game *game, bool flipped) {
	game->flipped = flipped;
	game->turn = WHITE;
	game->winner = NONE;
	game->started = false;

	// last move
	game->last_move.from.x = game->last_move.from.y =
	        game->last_move.to.x = game->last_move.to.y = 0;

	// reset captured pieces (NONE denotes end of array so we only need to set first)
	game->white_pieces_captured[0] = NONE;
	game->black_pieces_captured[0] = NONE;

	// initialize board
	for (uint8_t y = 0; y < BOARD_H; ++y) {
		for (uint8_t x = 0; x < BOARD_W; ++x) {
			struct piece *piece = &game->board[x][y];
			piece->type = NONE;
			piece->turns = 0;

			// set color and which direction pawns move
			piece->color = y < 4 ? WHITE : BLACK;

			if (y == 1 || y == BOARD_H - 2) {
				piece->type = PAWN;
			} else if (y == 0 || y == BOARD_H - 1) {
				switch (x) {
					case 0:
					case 7:
						piece->type = ROOK;
						break;
					case 1:
					case 6:
						piece->type = KNIGHT;
						break;
					case 2:
					case 5:
						piece->type = BISHOP;
						break;
					case 3:
						piece->type = QUEEN;
						break;
					case 4:
						piece->type = KING;
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
	array[i + 1] = NONE;
}

bool pos_is_valid(pos position) {
	return position.x >= 0 && position.x < BOARD_W &&
	       position.y >= 0 && position.y < BOARD_H;
}

pos get_view_pos(struct game *game, pos position) {
	if (game->flipped) {
		position.x = BOARD_W - position.x - 1;
		position.y = BOARD_H - position.y - 1;
	}
	return position;
}

struct piece *get_piece(struct game *game, pos position) {
	if (!pos_is_valid(position)) return NULL;
	return &game->board[position.x][position.y];
}

bool is_piece(struct piece piece, enum piece_color color) {
	return piece.type != NONE && piece.color == color;
}

bool is_legal_move_internal(struct game *game, move move, struct move_details *details);
bool move_piece_internal(struct game *game, move move, struct move_details *details);
void make_piece_move(struct game *game, move move, struct move_details details);

bool is_check(struct game *game, enum piece_color color) {
	// find king piece
	pos king_position;
	for (intpos x = 0; x < BOARD_W; ++x)
		for (intpos y = 0; y < BOARD_H; ++y) {
			struct piece *piece = get_piece(game, position(x, y));
			if (piece->type == KING && piece->color == color) {
				king_position = position(x, y);
				goto found_king;
			}
		}
	return true;

	struct move_details details = {0};
	details.check = true;

found_king:
	for (intpos x = 0; x < BOARD_W; ++x)
		for (intpos y = 0; y < BOARD_H; ++y) {
			pos piece_position = position(x, y);
			struct piece *piece = get_piece(game, piece_position);
			// if the king piece can be taken by any opponent piece
			if (piece->color != color)
				if (is_legal_move_internal(game, (struct move){.from = piece_position, .to = king_position}, &details))
					return true;
		}

	return false;
}

bool is_checkmate(struct game *game, enum piece_color color) {
	if (!is_check(game, color))
		return false;

	// loop pieces of player
	for (intpos x = 0; x < BOARD_W; ++x)
		for (intpos y = 0; y < BOARD_H; ++y) {
			pos piece_position = position(x, y);
			struct piece *piece = get_piece(game, piece_position);
			if (piece->color == color) {
				// loop available moves
				for (intpos x2 = 0; x2 < BOARD_W; ++x2)
					for (intpos y2 = 0; y2 < BOARD_H; ++y2) {
						pos new_position = position(x2, y2);
						struct game next_state = *game;
						// don't check for checkmate to prevent crash from infinite recursion
						if (move_piece_internal(&next_state, (struct move){.from = piece_position, .to = new_position}, NULL)) {
							// if the move gets the player out of check, it is not a checkmate
							if (!is_check(&next_state, color)) return false;
						}
					}
			}
		}

	// if no moves can get the player out of check, checkmate
	return true;
}

bool is_stalemate(struct game *game, enum piece_color color) {
	// loop pieces of player
	for (intpos x = 0; x < BOARD_W; ++x)
		for (intpos y = 0; y < BOARD_H; ++y) {
			pos piece_position = position(x, y);
			struct piece *piece = get_piece(game, piece_position);
			if (piece->color == color) {
				// loop available moves
				for (intpos x2 = 0; x2 < BOARD_W; ++x2)
					for (intpos y2 = 0; y2 < BOARD_H; ++y2) {
						pos new_position = position(x2, y2);
						if (is_legal_move_internal(game, (struct move){.from = piece_position, .to = new_position}, NULL))
							return false;
					}
			}
		}
	return true;
}

pos pos_dir(struct game *game, struct piece piece) {
	return position(0, piece.color ? -1 : 1);
}

bool is_clear_path(struct game *game, move move) {
	pos direction = pos_direction(move.from, move.to); // get direction
	if (direction.x == 0 && direction.y == 0) return false;
	// loop over all tiles from initial position to destination position
	for (pos line_pos = pos_add(move.from, direction);
	     true; line_pos = pos_add(line_pos, direction)) {
		struct piece *piece_line = get_piece(game, line_pos);
		if (!piece_line) break;
		if (pos_equal(line_pos, move.to)) return true;
		if (piece_line->type != NONE) break;
	}
	return false;
}

bool is_legal_move_internal(struct game *game, move move, struct move_details *details) {
	if (pos_equal(move.from, move.to)) return false;

	struct piece *piece_from = get_piece(game, move.from);
	struct piece *piece_to = get_piece(game, move.to);

	if (!piece_from) return false;
	if (!piece_to) return false;

	DETAILS(details);
	details->capture = details->en_passant = details->castle = details->promotion = false;

	pos distance = pos_subtract(move.to, move.from);
	pos abs_distance = pos_abs(distance);              // absolute value
	pos direction = pos_direction(move.from, move.to); // get direction

	if (is_piece(*piece_to, piece_from->color)) {
		// castling
		if (piece_from->type == KING && piece_to->type == ROOK)
			if (piece_from->turns == 0 && piece_to->turns == 0)            // cannot castle if the king or rook has moved
				if (!details->check && !is_check(game, piece_from->color)) // don't allow escaping from check
					if (is_clear_path(game, move)) {                       // castling only works if there are no spaces between the king and rook
						details->castle = true;
						goto validate_move;
					}
		return false;
	}

	switch (piece_from->type) {
		case KING:
			// return true if the destination position is a neighbor of the initial position
			if (pos_equal(distance, pos_normalize(distance))) goto validate_move;
			return false;
		case ROOK:
			// rook: return false if direction is not horizontal or vertical
			if (direction.x != 0 && direction.y != 0) return false;
		case BISHOP:
			if (piece_from->type == BISHOP) {
				// bishop: return false if direction is not diagonal
				if (direction.x == 0 || direction.y == 0) return false;
			}
		case QUEEN:
			if (is_clear_path(game, move)) goto validate_move;
			return false;
		case KNIGHT:
			// return true if distance is 1x2 or 2x1
			if ((abs_distance.x == 1 && abs_distance.y == 2) ||
			    (abs_distance.x == 2 && abs_distance.y == 1)) goto validate_move;
			return false;
		case PAWN:
			details->capture = piece_to->type != NONE;
			if (!details->capture) {
				// en passant
				struct piece *last_moved_piece = get_piece(game, game->last_move.to);
				pos last_move_distance = pos_abs(pos_subtract(game->last_move.to, game->last_move.from));
				if (last_moved_piece->type == PAWN &&               // only capture a pawn
				    last_moved_piece->color != piece_from->color && // only capture opponent piece
				    last_moved_piece->turns == 1 &&                 // pawn has only moved once
				    last_move_distance.y == 2 &&                    // pawn has moved two spaces, not one
				    game->last_move.to.x == move.to.x &&            // destination position has same column
				    game->last_move.to.y == move.from.y) {          // and initial position has same row
					details->en_passant = details->capture = true;
				}
			}

			if (details->capture) {
				// if we are taking a piece, return false if we are not moving to the neighboring column
				if (move.to.x + 1 != move.from.x &&
				    move.to.x - 1 != move.from.x) return false;
			} else {
				// if we are not taking a piece, return false if we are not moving to the same column
				if (move.to.x != move.from.x) return false;
			}

			pos dir = pos_dir(game, *piece_from);
			pos ahead1 = pos_add(move.from, dir);
			pos ahead2 = pos_add(move.from, pos_scale(dir, 2));

			// if we are not moving to the cell ahead
			if (ahead1.y != move.to.y &&
			    (move.from.x != move.to.x ||               // if we aren't in the same column,
			     piece_from->turns != 0 ||                 // we have moved,
			     ahead2.y != move.to.y ||                  // we are not moving 2 cells ahead, or
			     get_piece(game, ahead1)->type != NONE)) { // the piece 1 cell ahead isn't empty
				return false;                              // return false
			}

			if (move.to.y == (dir.y < 0 ? 0 : BOARD_H - 1)) {
				// promote the pawn once it reaches the other side of the board
				details->promotion = true;
			}

			goto validate_move;
		default:
			return false;
	}

validate_move:
	struct game next_state = *game;
	make_piece_move(&next_state, move, *details);
	if (is_check(&next_state, piece_from->color)) return false;
	if (piece_to->type != NONE && !details->castle) details->capture = true;
	return true;
}

bool is_legal_move(struct game *game, move move, struct move_details *details) {
	if (game->winner != NO_WINNER) return false;

	struct piece *piece_from = get_piece(game, move.from);
	if (!piece_from) return false;

	// check if we can move the piece in the first place
	if (piece_from->color != game->turn) return false;
	if (piece_from->type == NONE) return false;
	if (pos_equal(move.from, move.to)) {
		// check that the piece can move somewhere
		for (intpos x = 0; x < BOARD_W; ++x)
			for (intpos y = 0; y < BOARD_H; ++y) {
				if (is_legal_move_internal(game, (struct move){.from = move.from, .to = position(x, y)}, NULL))
					return true;
			}
		return false;
	}

	return is_legal_move_internal(game, move, details);
}

void make_piece_move(struct game *game, move move, struct move_details details) {
	struct piece *piece_from = get_piece(game, move.from);
	struct piece *piece_to = get_piece(game, move.to);

	// increment number of turns
	if (piece_from->turns < 2) ++piece_from->turns;

	// move the piece, TODO: castling and en passant
	piece_to->type = piece_from->type;
	piece_to->color = piece_from->color;
	piece_to->turns = piece_from->turns;
	piece_from->type = NONE;

	if (details.en_passant) {
		// take the other player's pawn
		struct piece *piece_last_moved = get_piece(game, game->last_move.to);
		piece_last_moved->type = NONE;
	}

	game->started = true;
	game->last_move = move;

	game->turn = OPPOSITE(game->turn);
}

bool move_piece_internal(struct game *game, move move, struct move_details *details) {
	if (pos_equal(move.from, move.to)) return false;
	DETAILS(details);
	if (is_legal_move(game, move, details)) {
		if (!details->promotion) make_piece_move(game, move, *details);
		return true;
	}
	return false;
}

bool move_piece_promote_internal(struct game *game, move move, struct move_details *details, enum piece_type type) {
	if (pos_equal(move.from, move.to)) return false;
	DETAILS(details);
	if (is_legal_move(game, move, details)) {
		if (details->promotion) {
			switch (type) {
				case QUEEN:
				case KNIGHT:
				case BISHOP:
				case ROOK:
					struct piece *piece_from = get_piece(game, move.from);
					piece_from->type = type;
					break;
				default:
					return false;
			}
		}
		make_piece_move(game, move, *details);
		return true;
	}
	return false;
}

//TODO: other ways of a draw
void check_game_win_condition(struct game *game) {
	if (game->winner != NO_WINNER) return;
	if (is_checkmate(game, WHITE)) game->winner = WINNER_BLACK;
	if (is_checkmate(game, BLACK)) game->winner = game->winner == NO_WINNER ? WINNER_WHITE : UNKNOWN;

	if (game->winner != NO_WINNER) return;
	if (is_stalemate(game, WHITE)) game->winner = STALEMATE;
	if (is_stalemate(game, BLACK)) game->winner = STALEMATE;
}

bool move_piece_promote(struct game *game, move move, struct move_details *details, enum piece_type type) {
	if (move_piece_promote_internal(game, move, details, type)) {
		check_game_win_condition(game);
		return true;
	}
	return false;
}

bool move_piece(struct game *game, move move, struct move_details *details) {
	if (move_piece_internal(game, move, details)) {
		check_game_win_condition(game);
		return true;
	}
	return false;
}
