#ifndef CONFIG_H
#define CONFIG_H
#include "./nuklear.h"
#define WINDOW_TITLE "Chess"
#define PROMOTING_WINDOW_TITLE "Promotion"

extern unsigned char binary_casefont_ttf[];
extern unsigned int binary_casefont_ttf_len;

#define LOAD_FONTS()                                                       \
	{                                                                      \
		load_font_system("sans-serif", ctx);                               \
		load_font_data(binary_casefont_ttf, binary_casefont_ttf_len, ctx); \
	}

#define DEFAULT_FONT(ctx, diagonal) set_font(0, (int) (diagonal * 14), ctx)
#define MENU_FONT(ctx, diagonal) set_font(0, (int) (diagonal * 10), ctx)
#define AXIS_FONT(ctx, diagonal) set_font(0, (int) (diagonal * 16), ctx)
#define CHESS_FONT(ctx, diagonal) set_font(1, (int) (diagonal * 30), ctx)

#define TEXT_BLACK_KING "l"
#define TEXT_BLACK_QUEEN "w"
#define TEXT_BLACK_ROOK "t"
#define TEXT_BLACK_BISHOP "v"
#define TEXT_BLACK_KNIGHT "m"
#define TEXT_BLACK_PAWN "o"

#define TEXT_WHITE_KING "l"
#define TEXT_WHITE_QUEEN "w"
#define TEXT_WHITE_ROOK "t"
#define TEXT_WHITE_BISHOP "v"
#define TEXT_WHITE_KNIGHT "m"
#define TEXT_WHITE_PAWN "o"

/*
#define TEXT_WHITE_KING "k"
#define TEXT_WHITE_QUEEN "q"
#define TEXT_WHITE_ROOK "r"
#define TEXT_WHITE_BISHOP "b"
#define TEXT_WHITE_KNIGHT "n"
#define TEXT_WHITE_PAWN "p"
*/

// you can find some themes in Nuklear/demo/common/style.c
#define THEME(theme, chess_theme)                                                                                       \
	{                                                                                                                   \
		theme[NK_COLOR_TEXT] = nk_rgba(70, 70, 70, 255);                                                                \
		theme[NK_COLOR_WINDOW] = nk_rgba(175, 175, 175, 255);                                                           \
		theme[NK_COLOR_HEADER] = nk_rgba(175, 175, 175, 255);                                                           \
		theme[NK_COLOR_BORDER] = nk_rgba(0, 0, 0, 255);                                                                 \
		theme[NK_COLOR_BUTTON] = nk_rgba(185, 185, 185, 255);                                                           \
		theme[NK_COLOR_BUTTON_HOVER] = nk_rgba(170, 170, 170, 255);                                                     \
		theme[NK_COLOR_BUTTON_ACTIVE] = nk_rgba(160, 160, 160, 255);                                                    \
		theme[NK_COLOR_TOGGLE] = nk_rgba(150, 150, 150, 255);                                                           \
		theme[NK_COLOR_TOGGLE_HOVER] = nk_rgba(120, 120, 120, 255);                                                     \
		theme[NK_COLOR_TOGGLE_CURSOR] = nk_rgba(175, 175, 175, 255);                                                    \
		theme[NK_COLOR_SELECT] = nk_rgba(190, 190, 190, 255);                                                           \
		theme[NK_COLOR_SELECT_ACTIVE] = nk_rgba(175, 175, 175, 255);                                                    \
		theme[NK_COLOR_SLIDER] = nk_rgba(190, 190, 190, 255);                                                           \
		theme[NK_COLOR_SLIDER_CURSOR] = nk_rgba(80, 80, 80, 255);                                                       \
		theme[NK_COLOR_SLIDER_CURSOR_HOVER] = nk_rgba(70, 70, 70, 255);                                                 \
		theme[NK_COLOR_SLIDER_CURSOR_ACTIVE] = nk_rgba(60, 60, 60, 255);                                                \
		theme[NK_COLOR_PROPERTY] = nk_rgba(175, 175, 175, 255);                                                         \
		theme[NK_COLOR_EDIT] = nk_rgba(150, 150, 150, 255);                                                             \
		theme[NK_COLOR_EDIT_CURSOR] = nk_rgba(0, 0, 0, 255);                                                            \
		theme[NK_COLOR_COMBO] = nk_rgba(175, 175, 175, 255);                                                            \
		theme[NK_COLOR_CHART] = nk_rgba(160, 160, 160, 255);                                                            \
		theme[NK_COLOR_CHART_COLOR] = nk_rgba(45, 45, 45, 255);                                                         \
		theme[NK_COLOR_CHART_COLOR_HIGHLIGHT] = nk_rgba(255, 0, 0, 255);                                                \
		theme[NK_COLOR_SCROLLBAR] = nk_rgba(180, 180, 180, 255);                                                        \
		theme[NK_COLOR_SCROLLBAR_CURSOR] = nk_rgba(140, 140, 140, 255);                                                 \
		theme[NK_COLOR_SCROLLBAR_CURSOR_HOVER] = nk_rgba(150, 150, 150, 255);                                           \
		theme[NK_COLOR_SCROLLBAR_CURSOR_ACTIVE] = nk_rgba(160, 160, 160, 255);                                          \
		theme[NK_COLOR_TAB_HEADER] = nk_rgba(180, 180, 180, 255);                                                       \
                                                                                                                        \
		chess_theme[CHESS_COLOR_1] = nk_rgba(160, 160, 160, 255);                                                       \
		chess_theme[CHESS_COLOR_2] = nk_rgba(185, 140, 80, 255);                                                        \
		chess_theme[CHESS_COLOR_MOVE] = nk_rgba(185, 80, 80, 255);                                                      \
		chess_theme[CHESS_COLOR_MOVE_AVAILABLE] = nk_rgba(140, 185, 80, 255);                                           \
		chess_theme[CHESS_COLOR_MOVE_CAPTURE] = nk_rgba(80, 180, 180, 255);                                             \
		chess_theme[CHESS_COLOR_MOVE_LAST] = nk_rgba(185, 180, 80, 255);                                                \
                                                                                                                        \
		chess_theme[CHESS_COLOR_1_HOVER] = multiply_color(chess_theme[CHESS_COLOR_1], 0.92);                            \
		chess_theme[CHESS_COLOR_2_HOVER] = multiply_color(chess_theme[CHESS_COLOR_2], 0.92);                            \
		chess_theme[CHESS_COLOR_MOVE_HOVER] = multiply_color(chess_theme[CHESS_COLOR_MOVE], 0.92);                      \
		chess_theme[CHESS_COLOR_MOVE_AVAILABLE_HOVER] = multiply_color(chess_theme[CHESS_COLOR_MOVE_AVAILABLE], 0.92);  \
		chess_theme[CHESS_COLOR_MOVE_CAPTURE_HOVER] = multiply_color(chess_theme[CHESS_COLOR_MOVE_CAPTURE], 0.92);      \
		chess_theme[CHESS_COLOR_MOVE_LAST_HOVER] = multiply_color(chess_theme[CHESS_COLOR_MOVE_LAST], 0.92);            \
                                                                                                                        \
		chess_theme[CHESS_COLOR_1_ACTIVE] = multiply_color(chess_theme[CHESS_COLOR_1], 0.86);                           \
		chess_theme[CHESS_COLOR_2_ACTIVE] = multiply_color(chess_theme[CHESS_COLOR_2], 0.86);                           \
		chess_theme[CHESS_COLOR_MOVE_ACTIVE] = multiply_color(chess_theme[CHESS_COLOR_MOVE], 0.86);                     \
		chess_theme[CHESS_COLOR_MOVE_AVAILABLE_ACTIVE] = multiply_color(chess_theme[CHESS_COLOR_MOVE_AVAILABLE], 0.86); \
		chess_theme[CHESS_COLOR_MOVE_CAPTURE_ACTIVE] = multiply_color(chess_theme[CHESS_COLOR_MOVE_CAPTURE], 0.86);     \
		chess_theme[CHESS_COLOR_MOVE_LAST_ACTIVE] = multiply_color(chess_theme[CHESS_COLOR_MOVE_LAST], 0.86);           \
                                                                                                                        \
		chess_theme[CHESS_COLOR_PIECE_BLACK] = nk_rgba(70, 70, 70, 255);                                                \
		chess_theme[CHESS_COLOR_PIECE_WHITE] = nk_rgba(225, 225, 225, 255);                                             \
	}
#endif //CONFIG_H
