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
#include "./util.h"

#define WINDOW_MENU "menu"
#define WINDOW_QUIT "quit"
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
	struct nk_font *font = NULL;
	struct nk_font *chess_font = NULL;

	{
		char *font_file = get_font_file(FONT_NAME);
		if (!font_file) {
			warnx("failed to get font: %s", FONT_NAME);
			goto exit;
		}

		FILE *font_fp = fopen(font_file, "rb");
		if (!font_fp) {
			warnx("failed to open font file: %s", font_file);
			goto exit;
		}

		void *font_file_data;
		size_t font_file_size;
		if (!read_file(font_fp, &font_file_data, &font_file_size)) {
			warnx("failed to read file: %s", font_file);
			fclose(font_fp);
			goto exit;
		}
		fclose(font_fp);

		font = load_font(font_file_data, font_file_size, FONT_SIZE, 1, ctx);
		nk_style_set_font(ctx, &font->handle);

		chess_font = load_font(CHESS_FONT_FILE, CHESS_FONT_FILE_LEN, CHESS_FONT_SIZE, 1, ctx);

		free(font_file_data);
	}
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
	game.state = MENU;
	bool is_moving = false;
	struct position moving = {0};

	bool loop = true;

	while (loop) {
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

		struct nk_rect menu_bounds = nk_rect(0, 0, 300, 300);
		move_rect_to_center(&menu_bounds, window_size);

		switch (game.state) {
			case PROMOTING:
				if (nk_begin_titled(ctx, WINDOW_MENU, WINDOW_TITLE,
				                    menu_bounds,
				                    NK_WINDOW_BORDER | NK_WINDOW_CLOSABLE | NK_WINDOW_NO_SCROLLBAR)) {
				}
				break;
			case GAME_OVER:
				if (nk_begin_titled(ctx, WINDOW_MENU, WINDOW_TITLE,
				                    menu_bounds,
				                    NK_WINDOW_BORDER | NK_WINDOW_CLOSABLE | NK_WINDOW_NO_SCROLLBAR)) {
				}
				break;
			case MENU:
				if (nk_begin_titled(ctx, WINDOW_MENU, WINDOW_TITLE,
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

					// draw buttons
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
					}
				} else if (game.state == MENU) {
					loop = false;
				}
				break;
			default:
				goto no_menu;
		}
		nk_end(ctx);
		nk_window_set_bounds(ctx, WINDOW_MENU, menu_bounds);

	no_menu:

		int grid_w = 4 + BOARD_W;
		int grid_h = 4 + BOARD_H;
		struct nk_rect main_bounds = get_fit_mode(window_size, nk_vec2(grid_w, grid_h));
		float grid_item_w = main_bounds.w / (float) grid_w;
		float grid_item_h = main_bounds.h / (float) grid_h;
		if (game.state != MENU && main_bounds.w > 0 && main_bounds.h > 0) {
			// main window
			struct nk_style style = ctx->style;
			ctx->style.button.border = 0;
			ctx->style.window.padding.x = 0;
			ctx->style.window.padding.y = 0;
			if (nk_begin_titled(ctx, WINDOW_MAIN, WINDOW_TITLE,
			                    main_bounds,
			                    NK_WINDOW_BACKGROUND | NK_WINDOW_CLOSABLE | NK_WINDOW_NO_SCROLLBAR)) {
				int board_x_min = 2;
				int board_y_min = 2;
				int board_x_max = BOARD_W + 1;
				int board_y_max = BOARD_H + 1;
				bool white_pieces_captures_done = false, black_pieces_captures_done = false;
				nk_layout_space_begin(ctx, NK_STATIC, INT_MAX, INT_MAX);
				for (int y = 0; y < grid_h; ++y) {
					for (int x = 0; x < grid_w; ++x) {
						if (x >= board_x_min && x <= board_x_max && y >= board_y_min && y <= board_y_max) {
							nk_layout_space_push(ctx, nk_rect(grid_item_w * x, grid_item_h * y, grid_item_w, grid_item_h));

							int board_x = x - board_x_min;
							int board_y = y - board_y_min;

							struct nk_style style_piece = ctx->style;
							struct piece piece = game.board[board_x][board_y];

							if ((board_x + board_y) & 1) {
								SET_BUTTON_BG_THEME(ctx->style.button, 2)
							} else {
								SET_BUTTON_BG_THEME(ctx->style.button, 1)
							}

							if (piece.color == BLACK) {
								SET_BUTTON_FG_THEME(ctx->style.button, PIECE_BLACK)
							} else {
								SET_BUTTON_FG_THEME(ctx->style.button, PIECE_WHITE)
							}

							nk_style_set_font(ctx, &chess_font->handle);
							if (draw_piece(ctx, piece, true)) {
								// clicked
							}

							ctx->style = style_piece;
						} else {
							struct nk_rect bounds = nk_rect(grid_item_w * x, grid_item_h * y, grid_item_w, grid_item_h);
							float margin = 10;

							bool x_label_sides = (x == board_x_min - 1 || x == board_x_max + 1) && y >= board_y_min && y <= board_y_max,
							     y_label_sides = (y == board_y_min - 1 || y == board_y_max + 1) && x >= board_x_min && x <= board_x_max,
							     x_captured_sides = x == board_x_min - 2,
							     y_captured_sides = x == board_x_max + 2;

							if (x_label_sides || y_label_sides) {
								bounds.x += margin;
								bounds.y += margin;
								bounds.w -= margin * 2;
								bounds.h -= margin * 2;
								nk_layout_space_push(ctx, bounds);

								nk_style_set_font(ctx, &font->handle);
								char label[3];
								sprintf(label, x_label_sides ? "%i" : "%c", x_label_sides ? board_y_max + 1 - y : x - board_x_min + 'A');

								nk_flags align = 0;
								if (x_label_sides) {
									align |= NK_TEXT_ALIGN_MIDDLE;
									if (x > board_x_min - 1) align |= NK_TEXT_ALIGN_LEFT;
									else
										align |= NK_TEXT_ALIGN_RIGHT;
								} else {
									align |= NK_TEXT_ALIGN_CENTERED;
									if (y > board_y_min - 1) align |= NK_TEXT_ALIGN_TOP;
									else
										align |= NK_TEXT_ALIGN_BOTTOM;
								}

								nk_label(ctx, label, align);
							} else if (x_captured_sides || y_captured_sides) {
								nk_layout_space_push(ctx, bounds);
								nk_style_set_font(ctx, &chess_font->handle);

								struct nk_style style_piece = ctx->style;

								if (x_captured_sides && !white_pieces_captures_done) {
									SET_BUTTON_FG_THEME(ctx->style.button, PIECE_WHITE)
									enum piece_type piece = game.white_pieces_captured[y];
									if (piece == NONE)
										white_pieces_captures_done = true;
									else
										draw_piece(ctx, (struct piece){.type = piece, .color = WHITE}, false);
								} else if (y_captured_sides && !black_pieces_captures_done) {
									SET_BUTTON_FG_THEME(ctx->style.button, PIECE_BLACK)
									enum piece_type piece = game.black_pieces_captured[y];
									if (piece == NONE)
										black_pieces_captures_done = true;
									else
										draw_piece(ctx, (struct piece){.type = piece, .color = BLACK}, false);
								}

								ctx->style = style_piece;
							}
						}
					}
				}
				nk_layout_space_end(ctx);
			} else {
				game.state = MENU;
			}
			nk_style_set_font(ctx, &font->handle);
			nk_end(ctx);
			ctx->style = style;
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
