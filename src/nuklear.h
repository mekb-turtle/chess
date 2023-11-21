#ifndef NUKLEAR_H
#define NUKLEAR_H
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include <stdbool.h>
#include "../Nuklear/nuklear.h"
#include "../Nuklear/demo/sdl_renderer/nuklear_sdl_renderer.h"
void move_rect_to_center(struct nk_rect *menu_bounds, struct nk_vec2 window_size);
struct nk_rect get_fit_mode(struct nk_vec2 window_size, struct nk_vec2 image_size);
#endif //NUKLEAR_H
