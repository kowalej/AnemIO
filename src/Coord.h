#pragma once

struct coord {
	coord(float v = NULL) : x(NAN), y(NAN), z(NAN) {}
	coord(float x, float y, float z) : x(x), y(y), z(z) {}

	float x;
	float y;
	float z;
};