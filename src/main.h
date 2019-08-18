#ifndef MAIN
#define MAIN

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <graphx.h>
#include <keypadc.h>
#include <debug.h>

#include "gfx.h"

struct character {
	uint8_t prev_y;
	uint8_t y;
	int8_t degree;
	uint8_t color;
};

#define PLAYER_X 64

#endif