#include "main.h"

void calculate_rotations();
void draw_main_menu();
void draw_square(int x, int y, uint8_t color);
void draw_rectangele(int x, int y, int width, int height, uint8_t color);
void draw_spike(int x, int y, int width, int height, uint8_t color);
void clear_screen();
void draw_fps(int rate);
void draw_level();
void draw_player_rotate(int x0, int y0, int angle, uint8_t color);