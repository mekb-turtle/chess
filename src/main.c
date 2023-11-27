#include <stdio.h>
#include <err.h>
#include <stdbool.h>
#include <getopt.h>
#include <time.h>
#include <limits.h>

#include <SDL2/SDL.h>
#include "./nuklear.h"
#include "./font.h"
#include "./chess.h"
#include "./config.h"
#include "./styling.h"

#define WINDOW_MENU "menu"
#define WINDOW_START "start"
#define WINDOW_MAIN "main"

static struct option options_getopt[] = {
        {"help",    no_argument, NULL, 'h'},
        {"version", no_argument, NULL, 'V'},
        {NULL,      0,           NULL, 0  }
};

enum player_color {
	PLAYER_WHITE,
	PLAYER_BLACK,
	PLAYER_RANDOM
};

enum player_mode {
	PLAYER_VS_PLAYER,
	PLAYER_VS_COMPUTER,
	COMPUTER_VS_COMPUTER,
};

float get_diagonal_size(float x, float y) {
	return sqrt(x * x + y * y);
}

int main(int argc, char *argv[]) {
	// region argument handling
	{
		bool invalid = false;

		int opt;

		while ((opt = getopt_long(argc, argv, ":hV", options_getopt, NULL)) != -1) {
			switch (opt) {
				case 'h':
					printf("Usage: %s [OPTION]...\n", TARGET);
					printf("-h --help: Shows help text\n");
					printf("-V --version: Shows the version\n");
					return 0;
				case 'V':
					printf("%s %s\n", TARGET, VERSION);
					return 0;
				default:
					invalid = true;
			}
		}

		if (optind != argc || invalid) errx(1, "Invalid usage, try --help");
	}
	// endregion

	int ret = 1;

	// region SDL initialization
	SDL_Window *window = NULL;
	SDL_Renderer *renderer = NULL;
	struct nk_context *ctx = NULL;

	{
		if (SDL_Init(SDL_INIT_VIDEO) != 0)
			errx(1, "SDL_Init: %s", SDL_GetError());

		window = SDL_CreateWindow(WINDOW_TITLE,
		                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		                          800, 600, SDL_WINDOW_SHOWN);
		if (!window) {
			warnx("SDL_CreateWindow: %s", SDL_GetError());
			goto exit;
		}

		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		if (!renderer) {
			warnx("SDL_CreateRenderer: %s", SDL_GetError());
			goto exit;
		}
	}
	// endregion

	ctx = nk_sdl_init(window, renderer);
	if (!ctx) {
		warnx("nk_sdl_init: %s", SDL_GetError());
		goto exit;
	}

	// region font loading
	LOAD_FONTS()
	// endregion

	// region theme
	struct nk_color chess_theme[CHESS_COLOR_COUNT] = {0};
	{
		struct nk_color nk_theme[NK_COLOR_COUNT] = {0};
		THEME(nk_theme, chess_theme);
		nk_style_from_table(ctx, nk_theme);
	}
	// endregion

	enum player_color player_color = PLAYER_RANDOM;
	enum player_mode player_mode = PLAYER_VS_COMPUTER;

	struct game game;
	bool is_playing = false;
	bool is_promoting = false;
	opt_move current_move = {0};

	bool loop = true;

	struct nk_style style_normal = ctx->style;
	struct nk_style style_board = ctx->style;
	style_board.button.border = 0;
	style_board.window.padding.x = 0;
	style_board.window.padding.y = 0;

	while (loop) {
		bool clicked = false; // prevent accidental double clicks

		SDL_Event event;

		nk_input_begin(ctx);
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				loop = false;
			}
			nk_sdl_handle_event(&event);
		}
		nk_input_end(ctx);

		// get window size
		int window_width_, window_height_;
		SDL_GetRendererOutputSize(renderer, &window_width_, &window_height_);
		struct nk_vec2 window_size = nk_vec2(window_width_, window_height_);

		if (is_promoting && (!current_move.to.has_value || !current_move.from.has_value)) is_promoting = false;
		if (!is_promoting && current_move.to.has_value) current_move.to.has_value = false;

		int grid_w = 4 + BOARD_W;
		int grid_h = 4 + BOARD_H;

		struct nk_rect main_bounds = get_fit_mode(window_size, nk_vec2(grid_w, grid_h));
		float main_diagonal_size = get_diagonal_size(main_bounds.w, main_bounds.h) / 600;

		struct nk_vec2 grid_item = nk_vec2(main_bounds.w / (float) grid_w, main_bounds.h / (float) grid_h);
		if (is_playing) {
			// main window
			ctx->style = style_board;
			DEFAULT_FONT(ctx, main_diagonal_size);
			if (nk_begin_titled(ctx, WINDOW_MAIN, WINDOW_TITLE,
			                    main_bounds,
			                    NK_WINDOW_BACKGROUND | NK_WINDOW_CLOSABLE | NK_WINDOW_NO_SCROLLBAR)) {
				pos board_min = {2, 2};
				pos board_max = {BOARD_W + 1, BOARD_H + 1};
				bool white_pieces_captures_done = false, black_pieces_captures_done = false;
				nk_layout_space_begin(ctx, NK_STATIC, INT_MAX, INT_MAX);
				for (int y = 0; y < grid_h; ++y) {
					for (int x = 0; x < grid_w; ++x) {
						if (x >= board_min.x && x <= board_max.x && y >= board_min.y && y <= board_max.y) {
							struct nk_rect tile_bounds = nk_rect(grid_item.x * x, grid_item.y * y, grid_item.x, grid_item.y);
							nk_layout_space_push(ctx, tile_bounds);

							pos pos = {x - board_min.x, board_max.y - y};

							struct piece piece = game.board[pos.x][pos.y];

							if (pos_equal_opt1(pos, current_move.from)) {
								SET_BUTTON_BG_THEME(ctx->style.button, MOVE1)
							} else if (pos_equal_opt1(pos, current_move.to)) {
								SET_BUTTON_BG_THEME(ctx->style.button, MOVE1)
							} else if (current_move.from.has_value && is_legal_move(&game, movement(current_move.from.value, pos), NULL)) {
								SET_BUTTON_BG_THEME(ctx->style.button, MOVE2)
							} else if ((pos.x + pos.y) & 1) {
								SET_BUTTON_BG_THEME(ctx->style.button, 2)
							} else {
								SET_BUTTON_BG_THEME(ctx->style.button, 1)
							}

							if (piece.color == BLACK) {
								SET_BUTTON_FG_THEME(ctx->style.button, PIECE_BLACK)
							} else {
								SET_BUTTON_FG_THEME(ctx->style.button, PIECE_WHITE)
							}

							CHESS_FONT(ctx, main_diagonal_size);
							if (draw_piece(ctx, piece.type, piece.color, true) && game.winner == NO_WINNER && !clicked) {
								is_promoting = false;
								clicked = true;
								struct move_details details = {0};
								// clicked tile
								if (pos_equal_opt1(pos, current_move.from)) {
									// cancel moving if we clicked the tile we're moving from
									current_move.from.has_value = false;
								} else if (current_move.from.has_value && move_piece(&game, movement(current_move.from.value, pos), &details)) {
									if (game.winner == NO_WINNER && details.promotion) {
										// open the dialog
										current_move.to.value = pos;
										current_move.to.has_value = true;
										is_promoting = true;
									} else {
										// cancel moving after we've moved
										current_move.from.has_value = false;
									}
								} else if (is_legal_move(&game, movement(pos, pos), NULL)) {
									// set the piece we're moving
									current_move.from.value = pos;
									current_move.from.has_value = true;
								} else {
									// cancel moving if we can't move this piece
									current_move.from.has_value = false;
								}
							}

							ctx->style = style_board;
							DEFAULT_FONT(ctx, main_diagonal_size);
						} else {
							struct nk_rect bounds = nk_rect(grid_item.x * x, grid_item.y * y, grid_item.x, grid_item.y);
							float margin = grid_item.x * 0.1;

							bool x_label_sides = (x == board_min.x - 1 || x == board_max.x + 1) && y >= board_min.y && y <= board_max.y,
							     y_label_sides = (y == board_min.y - 1 || y == board_max.y + 1) && x >= board_min.x && x <= board_max.x,
							     x_captured_sides = x == board_min.x - 2,
							     y_captured_sides = x == board_max.x + 2;

							if (x_label_sides || y_label_sides) {
								bounds.x += margin;
								bounds.y += margin;
								bounds.w -= margin * 2;
								bounds.h -= margin * 2;
								nk_layout_space_push(ctx, bounds);

								// draw grid labels
								DEFAULT_FONT(ctx, main_diagonal_size);
								char label[3];
								sprintf(label, x_label_sides ? "%i" : "%c", x_label_sides ? board_max.y - y + 1 : x - board_min.x + 'A');

								nk_flags align = 0;
								if (x_label_sides) {
									align |= NK_TEXT_ALIGN_MIDDLE;
									align |= x > board_min.x - 1 ? NK_TEXT_ALIGN_LEFT : NK_TEXT_ALIGN_RIGHT;
								} else {
									align |= NK_TEXT_ALIGN_CENTERED;
									align |= y > board_min.y - 1 ? NK_TEXT_ALIGN_TOP : NK_TEXT_ALIGN_BOTTOM;
								}

								nk_label(ctx, label, align);
							} else if (x_captured_sides || y_captured_sides) {
								nk_layout_space_push(ctx, bounds);
								CHESS_FONT(ctx, main_diagonal_size);

								if (x_captured_sides && !white_pieces_captures_done) {
									SET_BUTTON_FG_THEME(ctx->style.button, PIECE_WHITE)
									enum piece_type piece = game.white_pieces_captured[y];
									if (piece == NONE)
										white_pieces_captures_done = true;
									else
										draw_piece(ctx, piece, WHITE, false);
								} else if (y_captured_sides && !black_pieces_captures_done) {
									SET_BUTTON_FG_THEME(ctx->style.button, PIECE_BLACK)
									enum piece_type piece = game.black_pieces_captured[y];
									if (piece == NONE)
										black_pieces_captures_done = true;
									else
										draw_piece(ctx, piece, BLACK, false);
								}

								ctx->style = style_board;
								DEFAULT_FONT(ctx, main_diagonal_size);
							}
						}
					}
				}
				nk_layout_space_end(ctx);
			} else {
				is_playing = false;
			}
			nk_end(ctx);
			ctx->style = style_normal;
			DEFAULT_FONT(ctx, main_diagonal_size);
		}

		if (is_playing) {
			if (game.winner != NO_WINNER) { // winner menu
				struct nk_rect menu_bounds = nk_rect(0, 0, main_diagonal_size * 300, main_diagonal_size * 300);
				menu_bounds.w = fminf(fmaxf(menu_bounds.w, 360), window_size.x);
				menu_bounds.h = fminf(fmaxf(menu_bounds.h, 360), window_size.y);

				move_rect_to_center(&menu_bounds, window_size);
				float menu_diagonal_size = get_diagonal_size(menu_bounds.w, menu_bounds.h) / 600;
				nk_window_set_bounds(ctx, WINDOW_MENU, menu_bounds);
				DEFAULT_FONT(ctx, menu_diagonal_size);
				if (nk_begin_titled(ctx, WINDOW_MENU, WINDOW_TITLE,
				                    menu_bounds,
				                    NK_WINDOW_BORDER | NK_WINDOW_CLOSABLE | NK_WINDOW_NO_SCROLLBAR)) {
					char *text = NULL;
					switch (game.winner) {
						case WINNER_WHITE:
							text = "White wins";
							break;
						case WINNER_BLACK:
							text = "Black wins";
							break;
						case STALEMATE:
							text = "Stalemate";
							break;
						default:
							text = "Unknown";
							break;
					}
					if (text != NULL) {
						nk_layout_row_dynamic(ctx, 0, 1);
						nk_label(ctx, text, NK_TEXT_CENTERED);
					}
				} else {
					clicked = true;
					is_playing = false;
					nk_window_show(ctx, WINDOW_MENU, 0);
				}
				nk_end(ctx);
			} else {
				if (is_promoting) { // promotion menu
					float margin = grid_item.x * 0.1;
					ctx->style = style_board;

					struct nk_rect menu_bounds = {0};
					menu_bounds.w = grid_item.x * 2 + margin * 3 + ctx->style.window.border;
					menu_bounds.h = grid_item.y * 2 + margin * 3 + ctx->style.window.border;
					move_rect_to_center(&menu_bounds, window_size);

					if (current_move.to.value.y < 4) {
						menu_bounds.y += grid_item.y * 1;
					}

					float menu_diagonal_size = get_diagonal_size(menu_bounds.w, menu_bounds.h) / 600;
					nk_window_set_bounds(ctx, WINDOW_MENU, menu_bounds);
					DEFAULT_FONT(ctx, menu_diagonal_size);
					nk_window_set_bounds(ctx, WINDOW_MENU, menu_bounds);

					if (nk_begin_titled(ctx, WINDOW_MENU, PROMOTING_WINDOW_TITLE,
					                    menu_bounds,
					                    NK_WINDOW_BORDER | NK_WINDOW_CLOSABLE | NK_WINDOW_NO_SCROLLBAR)) {

						struct piece *piece = get_piece(&game, current_move.from.value);

						if (piece->color == BLACK) {
							SET_BUTTON_FG_THEME(ctx->style.button, PIECE_BLACK)
						} else {
							SET_BUTTON_FG_THEME(ctx->style.button, PIECE_WHITE)
						}

						if ((current_move.to.value.x + current_move.to.value.y) & 1) {
							SET_BUTTON_BG_THEME(ctx->style.button, 2)
						} else {
							SET_BUTTON_BG_THEME(ctx->style.button, 1)
						}

						CHESS_FONT(ctx, main_diagonal_size);

						nk_layout_space_begin(ctx, NK_STATIC, INT_MAX, INT_MAX);

						enum piece_type type = NONE;

						nk_layout_space_push(ctx, nk_rect(margin, margin, grid_item.x, grid_item.y));
						if (draw_piece(ctx, QUEEN, piece->color, true)) type = QUEEN;

						nk_layout_space_push(ctx, nk_rect(grid_item.x + margin * 2, margin, grid_item.x, grid_item.y));
						if (draw_piece(ctx, KNIGHT, piece->color, true)) type = KNIGHT;

						nk_layout_space_push(ctx, nk_rect(margin, grid_item.y + margin * 2, grid_item.x, grid_item.y));
						if (draw_piece(ctx, BISHOP, piece->color, true)) type = BISHOP;

						nk_layout_space_push(ctx, nk_rect(grid_item.x + margin * 2, grid_item.y + margin * 2, grid_item.x, grid_item.y));
						if (draw_piece(ctx, ROOK, piece->color, true)) type = ROOK;

						if (type != NONE) {
							clicked = true;
							is_promoting = false;
							current_move.to.has_value = false;
							if (move_piece_promote(&game, (struct move){.from = current_move.from.value, .to = current_move.to.value}, NULL, type)) {
								current_move.from.has_value = false;
							}
						}

						nk_layout_space_end(ctx);

					} else {
						clicked = true;
						current_move.to.has_value = false;
						is_promoting = false;
						nk_window_show(ctx, WINDOW_MENU, 0);
					}
					nk_end(ctx);
					ctx->style = style_normal;
					DEFAULT_FONT(ctx, menu_diagonal_size);
				}
			}
		}
		if (!clicked && !is_playing) { // start game menu
			struct nk_rect menu_bounds = nk_rect(0, 0, main_diagonal_size * 300, main_diagonal_size * 300);
			menu_bounds.w = fminf(fmaxf(menu_bounds.w, 360), window_size.x);
			menu_bounds.h = fminf(fmaxf(menu_bounds.h, 360), window_size.y);

			move_rect_to_center(&menu_bounds, window_size);
			float menu_diagonal_size = get_diagonal_size(menu_bounds.w, menu_bounds.h) / 600;
			nk_window_set_bounds(ctx, WINDOW_MENU, menu_bounds);
			DEFAULT_FONT(ctx, menu_diagonal_size);
			if (nk_begin_titled(ctx, WINDOW_START, WINDOW_TITLE,
			                    menu_bounds,
			                    NK_WINDOW_BORDER | NK_WINDOW_CLOSABLE | NK_WINDOW_NO_SCROLLBAR)) {
				nk_layout_row_dynamic(ctx, 0, 1);
				nk_label(ctx, "Select color", NK_TEXT_CENTERED);
				nk_layout_row_dynamic(ctx, 0, 3);
				if (nk_option_label(ctx, "Black", player_color == PLAYER_BLACK))
					player_color = PLAYER_BLACK;
				if (nk_option_label(ctx, "Random", player_color == PLAYER_RANDOM))
					player_color = PLAYER_RANDOM;
				if (nk_option_label(ctx, "White", player_color == PLAYER_WHITE))
					player_color = PLAYER_WHITE;

				nk_layout_row_dynamic(ctx, 0, 1);
				nk_label(ctx, "Select players", NK_TEXT_CENTERED);
				nk_layout_row_dynamic(ctx, 0, 1);
				if (nk_option_label(ctx, "Player vs. Player", player_mode == PLAYER_VS_PLAYER))
					player_mode = PLAYER_VS_PLAYER;
				if (nk_option_label(ctx, "Player vs. Computer", player_mode == PLAYER_VS_COMPUTER))
					player_mode = PLAYER_VS_COMPUTER;
				if (nk_option_label(ctx, "Computer vs. Computer", player_mode == COMPUTER_VS_COMPUTER))
					player_mode = COMPUTER_VS_COMPUTER;

				// draw button
				nk_layout_row_dynamic(ctx, 0, 1);
				if (nk_button_label(ctx, "Start")) {
					enum piece_color bottom;
					switch (player_color) {
						case PLAYER_WHITE:
							bottom = WHITE;
							break;
						case PLAYER_BLACK:
							bottom = BLACK;
							break;
						default:
							srand(time(NULL));
							bottom = rand() & 1 ? WHITE : BLACK;
							break;
					}
					// start game
					start_game(&game, bottom);
					current_move.from.has_value = false;
					is_playing = true;
					is_promoting = false;
				}
			} else {
				clicked = true;
				loop = false;
			}
			nk_end(ctx);
		}

		// draw
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		nk_sdl_render(NK_ANTI_ALIASING_ON);
		SDL_RenderPresent(renderer);
	}

	ret = 0;
exit:
	if (ctx) nk_sdl_shutdown();
	if (renderer) SDL_DestroyRenderer(renderer);
	if (window) SDL_DestroyWindow(window);
	SDL_Quit();

	return ret;
}
