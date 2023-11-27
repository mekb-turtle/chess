#ifndef UTIL_H
#define UTIL_H
#include <stdint.h>
#include <stdbool.h>

typedef int8_t intpos;

typedef struct pos {
	intpos x, y;
} pos;

typedef struct opt_pos {
	pos value;
	bool has_value;
} opt_pos;

typedef struct move {
	pos from;
	pos to;
} move;

typedef struct opt_move {
	opt_pos from;
	opt_pos to;
} opt_move;

pos position(intpos x, intpos y);
move movement(pos from, pos to);
bool pos_equal(pos a, pos b);
bool pos_equal_opt1(pos a, opt_pos b);
bool pos_equal_opt2(opt_pos a, opt_pos b);
intpos intpos_sign(intpos position);
pos pos_sign(pos position);
pos pos_add(pos a, pos b);
pos pos_subtract(pos a, pos b);
pos pos_scale(pos a, intpos t);
pos pos_direction(pos a, pos b);
intpos intpos_abs(intpos value);
pos pos_abs(pos position);
#endif //UTIL_H
