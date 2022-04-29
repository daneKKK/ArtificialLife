#pragma once

struct pixel {
	bool counted = false;
	float cost = 0;
	char type = '0';
	int vect[2] = { 0, 0 };
};