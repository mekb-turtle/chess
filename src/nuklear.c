#define NK_IMPLEMENTATION
#define NK_SDL_RENDERER_IMPLEMENTATION
#include "nuklear.h"
void move_rect_to_center(struct nk_rect *menu_bounds, struct nk_vec2 window_size) {
	menu_bounds->x = window_size.x / 2 - menu_bounds->w / 2;
	menu_bounds->y = window_size.y / 2 - menu_bounds->h / 2;
}
struct nk_rect get_fit_mode(struct nk_vec2 window_size, struct nk_vec2 image_size) {
	// gets the fit mode rect for an image and window size

	float scale;

	if (image_size.x == 0.0f || image_size.y == 0.0f || window_size.x == 0.0f || window_size.y == 0.0f)
		return nk_rect(0, 0, 0, 0);

	// get fit mode rect from image and window size
	if (image_size.x / image_size.y > window_size.x / window_size.y) {
		scale = window_size.x / image_size.x;
		return nk_rect(
		        0, (window_size.y * 0.5f) - (image_size.y * scale * 0.5f),
		        window_size.x, scale * image_size.y);
	} else {
		scale = window_size.y / image_size.y;
		return nk_rect(
		        (window_size.x * 0.5f) - (image_size.x * scale * 0.5f), 0,
		        scale * image_size.x, window_size.y);
	}
}
