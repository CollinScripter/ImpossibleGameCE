#include "main.h"

void init_gfx();
void draw_start_level();
void draw_player_rotate(int x, int y, int angle, uint8_t color);
void calculate_rotations();
void swap_draw();
void draw_coordinates();
void draw_next_level(uint8_t movement);
void draw_fps(int rate);

void draw_rectangle(int x, int y, int width, int height, uint8_t color);
void draw_spike(int x, int y, int width, int height, uint8_t color);
void draw_square(int x, int y, int size, uint8_t color);