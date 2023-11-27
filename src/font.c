#include "font.h"
#include <fontconfig/fontconfig.h>
#include <stdio.h>
#include "err.h"

char *get_font_file(char *pattern) {
	FcPattern *fc_pattern = FcNameParse((const FcChar8 *) pattern);
	FcConfig *config = FcInitLoadConfigAndFonts();
	FcConfigSubstitute(config, fc_pattern, FcMatchPattern);
	FcDefaultSubstitute(fc_pattern);

	FcResult result;
	FcPattern *font = FcFontMatch(config, fc_pattern, &result);
	if (font) {
		FcChar8 *file = NULL;
		if (FcPatternGetString(font, FC_FILE, 0, &file) == FcResultMatch) {
			return (char *) file;
		}
	}

	return NULL;
}

struct nk_font *load_font(void *font_file_data, size_t font_file_size, float font_size, float font_scale, struct nk_context *ctx) {
	struct nk_font_atlas *atlas;
	nk_sdl_font_stash_begin(&atlas);
	struct nk_font_config config = nk_font_config(0);
	struct nk_font *font = nk_font_atlas_add_from_memory(atlas, font_file_data, font_file_size, font_size * font_scale, &config);
	nk_sdl_font_stash_end();
	font->handle.height /= font_scale;
	return font;
}

bool read_file(FILE *fp, void **data_, size_t *size_) {
	// read a file in chunks
	const size_t read_size = 0x2000;
	const size_t alloc_mul = 2;

	size_t capacity = read_size;
	void *data = malloc(capacity);
	if (!data) {
		warn("malloc");
		return false;
	}

	size_t offset = 0;
	size_t read;
	while ((read = fread(data + offset, 1, read_size, fp)) > 0) {
		offset += read;
		if (offset + read_size > capacity) {
			capacity *= alloc_mul;
			data = realloc(data, capacity);
			if (!data) {
				warn("realloc");
				return false;
			}
		}
	}

	if (ferror(fp)) {
		free(data);
		warnx("ferror");
		return false;
	}

	*data_ = data;
	*size_ = offset;
	return true;
}

#define MAX_FONT_SIZE 100

struct nk_font *fonts[16][MAX_FONT_SIZE];
size_t font_len = 0;

void set_font(int font_index, int font_size, struct nk_context *ctx) {
	int size_index = font_size - 1;
	if (size_index < 0) size_index = 0;
	if (size_index > MAX_FONT_SIZE - 1) size_index = MAX_FONT_SIZE - 1;
	nk_style_set_font(ctx, &fonts[font_index][size_index]->handle);
}

bool load_font_data(void *data, size_t size, struct nk_context *ctx) {
	for (int i = 0; i < MAX_FONT_SIZE; ++i) {
		fonts[font_len][i] = load_font(data, size, i + 1, 1, ctx);
	}
	++font_len;
	return true;
}

bool load_font_file(char *font_file, struct nk_context *ctx) {
	FILE *font_fp = fopen(font_file, "rb");
	if (!font_fp) {
		warnx("failed to open font file: %s", font_file);
		return false;
	}

	void *font_file_data;
	size_t font_file_size;
	if (!read_file(font_fp, &font_file_data, &font_file_size)) {
		warnx("failed to read file: %s", font_file);
		fclose(font_fp);
		return false;
	}
	fclose(font_fp);
	bool result = load_font_data(font_file_data, font_file_size, ctx);
	free(font_file_data);
	return result;
}

bool load_font_system(char *font_name, struct nk_context *ctx) {
	char *font_file = get_font_file(font_name);
	if (!font_file) {
		warnx("failed to get font: %s", font_name);
		return false;
	}

	return load_font_file(font_file, ctx);
}
