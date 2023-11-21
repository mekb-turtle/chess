#ifndef FONT_H
#define FONT_H
#include <stdbool.h>
#include <stdio.h>
#include "nuklear.h"
char *get_font_file(char *pattern);
struct nk_font *load_font(void *font_file_data, size_t font_file_size, float font_size, float font_scale, struct nk_context *ctx);
bool read_file(FILE *fp, void **data, size_t *size);
#endif //FONT_H
