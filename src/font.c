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
