#include "./util.h"

pos position(intpos x, intpos y) {
	return (pos){.x = x, .y = y};
}

move movement(pos from, pos to) {
	return (move){.from = from, .to = to};
}

bool pos_equal(pos a, pos b) {
	return a.x == b.x && a.y == b.y;
}

bool pos_equal_opt1(pos a, opt_pos b) {
	return b.has_value && pos_equal(a, b.value);
}

bool pos_equal_opt2(opt_pos a, opt_pos b) {
	return a.has_value && b.has_value && pos_equal(a.value, b.value);
}

intpos intpos_sign(intpos position) {
	if (position < -1) position = -1;
	if (position > 1) position = 1;
	return position;
}

pos pos_sign(pos position) {
	position.x = intpos_sign(position.x);
	position.y = intpos_sign(position.y);
	return position;
}

pos pos_add(pos a, pos b) {
	return position(a.x + b.x, a.y + b.y);
}

pos pos_subtract(pos a, pos b) {
	return position(a.x - b.x, a.y - b.y);
}

pos pos_scale(pos a, intpos t) {
	return position(a.x * t, a.y * t);
}

pos pos_direction(pos a, pos b) {
	// calculate the direction of the distance
	// returns 0,0 if the two positions aren't in a straight line from each other (either horizontally, vertically or diagonally)
	pos distance = pos_subtract(b, a);
	if (distance.x == distance.y || distance.x == -distance.y ||
	    distance.x == 0 || distance.y == 0) {
		return pos_sign(distance);
	}
	return position(0, 0);
}

intpos intpos_abs(intpos value) {
	return value < 0 ? -value : value;
}

pos pos_abs(pos pos) {
	return position(intpos_abs(pos.x), intpos_abs(pos.y));
}
