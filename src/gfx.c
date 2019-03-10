//http://ce-programming.github.io/toolchain/graphx_8h.html
//https://github.com/CE-Programming/toolchain/wiki/GraphX-Library:-Partial-Redraw

#include "gfx.h"

int rotate[4][12][2] = {0};//Bottom left corner
uint8_t current_color; //It may or may not be faster to check if we've changed colors or change it, time it.
uint8_t background_color;
uint8_t frame_tracker[2];
uint8_t draw_location = 0; //0 is screen, 1 is buffer.
extern char level[7][276];
extern uint16_t position[2];
extern uint8_t debug;

void init_gfx() {
	gfx_Begin();
	gfx_SetDrawBuffer();
    background_color = gfx_RGBTo1555(0x60, 0x60, 0xFF);
    gfx_SetColor(gfx_blue);
    calculate_rotations();
}

void swap_draw() {
	gfx_SwapDraw();
	draw_location = draw_location ? 0 : 1;
}

void calculate_rotations() {
	int point[2] = {0};
	int i, j;
	float angle, s, c;
	for (i = 0; i < 4; i++) {
		switch(i) {
			case 0: //Top Left
				point[0] = -16;
				point[1] = -16;
				break;
			case 1: //Top Right
				point[0] = 16;
				point[1] = -16;
				break;
			case 2: //Bottom Left
				point[0] = -16;
				point[1] = 16;
				break;
			case 3: //Bottom Right
				point[0] = 16;
				point[1] = 16;
				break;
		}
		for (j = 0; j < 12; j++) {
			angle = 0.1208304867 * (j + 1);
			s = sin(angle);
			c = cos(angle);
			rotate[i][j][0] = point[0] * c - point[1] * s;
			rotate[i][j][1] = point[0] * s + point[1] * c;
		}
	}
}

void draw_square(int x, int y, int size, uint8_t color) {
	gfx_SetColor(color);
	gfx_FillRectangle(x, y, size, size);
}

void draw_rectangle(int x, int y, int width, int height, uint8_t color) {
	gfx_SetColor(color);
	gfx_FillRectangle(x, y, width, height);
}

void draw_spike(int x, int y, int width, int height, uint8_t color) {
	gfx_SetColor(color);
	gfx_FillTriangle(x, y + height - 1, x + width / 2, y, x + width - 1, y + height - 1);
}

void clear_screen() {
	gfx_FillScreen(background_color);
}

void draw_player_rotate(int x, int y, int angle, uint8_t color) {
	gfx_SetColor(color);
	if (angle == 0) {
		draw_square(x, y, 32, gfx_orange);
	} else {
		angle--; //Makes the range 0-12
		y += 16; //Draws from middle
		x += 16; //Draws from middle
		gfx_FillTriangle(rotate[0][angle][0] + x, rotate[0][angle][1] + y, rotate[1][angle][0] + x, rotate[1][angle][1] + y, rotate[2][angle][0] + x, rotate[2][angle][1] + y);
		gfx_FillTriangle(rotate[1][angle][0] + x, rotate[1][angle][1] + y, rotate[2][angle][0] + x, rotate[2][angle][1] + y, rotate[3][angle][0] + x, rotate[3][angle][1] + y);
	}
}

void draw_start_level() { //Draws the initial stage area
    uint8_t x, y;
	//char tile[8];

	clear_screen();
	draw_rectangle(0, LCD_HEIGHT - 16, LCD_WIDTH, 4, gfx_white);

    for (y = 0; y < 7; y++) {
		for (x = 0; x < 10; x++) {
			switch (level[y][x]) {
				case ' ': //Empty
					break;
				case 'D': //Square Block
					draw_square(x * 32, y * 32, 32, gfx_blue);
					break;
				case 'x': //Small Spike
					draw_spike(x * 32, y * 32 + 16, 32, 16, gfx_red);
					break;
				case 'X': //Large Spike
					draw_spike(x * 32, y * 32, 32, 32, gfx_red);
					break;
				case 's': //Two Small Spikes
					draw_spike(x * 32, y * 32 + 16, 16, 16, gfx_red);
					draw_spike(x * 32 + 16, y * 32 + 16, 16, 16, gfx_red);
					break;
				case '-': //Top Half Block
					draw_rectangle(x * 32, y * 32, 32, 16, gfx_blue);
					break;
				case 'F': //Finish Line
					draw_square(x * 32, y * 32, 32, gfx_yellow);
					break;
			}

			//if (debug) {
			//	gfx_SetColor(gfx_green);
			//	gfx_Rectangle(x * 32, y * 32, 32, 32);
			//	sprintf(tile, " %d ", x);
			//	gfx_SetTextFGColor(gfx_black);
			//	gfx_SetTextBGColor(gfx_pink);
			//	gfx_PrintStringXY(tile, x * 32 + 8, 8);
			//}
		}
	}
}

void draw_coordinates() {
	int y;
	char tile[8];
	for (y = 0; y < 7; y++) {
		sprintf(tile, "  %d ", y);
		gfx_SetTextFGColor(gfx_black);
		gfx_SetTextBGColor(gfx_pink);
		gfx_PrintStringXY(tile, 0, y * 32 + 16);
	}
}

void draw_fps(int rate) {
	char returnframerate[14];
	sprintf(returnframerate, "  FPS: %d", rate);
	gfx_SetTextFGColor(gfx_black);
	gfx_SetTextBGColor(background_color);
	gfx_PrintStringXY(returnframerate, 0, 4);
}

void draw_next_level(uint8_t movement) { //Draws the next stage area
    uint8_t y = 7;
	//char tile[8];
	
    if (movement) {
		gfx_ShiftLeft(movement); //Move left
		draw_rectangle(LCD_WIDTH - movement, 0, movement, 224, background_color); //Blank out previous drawing area
		draw_rectangle(LCD_WIDTH - movement, LCD_HEIGHT - 16, movement, 4, gfx_white); //Draw line
		frame_tracker[draw_location] += movement;
	}

    while (--y) {
		switch (level[y][10 + position[draw_location]]) {
			case ' ': //Empty
				break;
			case 'D': //Square Block
				draw_square(320 - frame_tracker[draw_location], y * 32, 32, gfx_blue);
				break;
			case 'x': //Small Spike
				draw_spike(320 - frame_tracker[draw_location], y * 32 + 16, 32, 16, gfx_red);
				break;
			case 'X': //Large Spike
				draw_spike(320 - frame_tracker[draw_location], y * 32, 32, 32, gfx_red);
				break;
			case 's': //Two Small Spikes
				draw_spike(320 - frame_tracker[draw_location], y * 32 + 16, 16, 16, gfx_red);
				draw_spike(336 - frame_tracker[draw_location], y * 32 + 16, 16, 16, gfx_red); //320 + 16
				break;
			case '-': //Top Half Block
				draw_rectangle(320 - frame_tracker[draw_location], y * 32, 32, 16, gfx_blue);
				break;
			case 'F': //Finish Line
				draw_square(320 - frame_tracker[draw_location], y * 32, 32, gfx_yellow);
				break;
		}
		
		//if (debug) {
		//	dbg_sprintf(dbgout, "Position: %u\n", position[draw_location]);
		//	dbg_sprintf(dbgout, "Movement: %u\n", movement);
		//	dbg_sprintf(dbgout, "Draw Location %u\n", draw_location);
		//	dbg_sprintf(dbgout, "Frame Tracker: %u\n", frame_tracker[draw_location]);
		//	gfx_SetColor(gfx_green);
		//	gfx_Rectangle(x * 32 - frame_tracker[draw_location], y * 32, 32, 32);
		//	sprintf(tile, " %d ", x + position[draw_location]);
		//	gfx_SetTextFGColor(gfx_black);
		//	gfx_SetTextBGColor(gfx_pink);
		//	gfx_PrintStringXY(tile, x * 32 + 8 - frame_tracker[draw_location], 8);
		//}
	}
	
	if (frame_tracker[draw_location] >= 32) {
		position[draw_location]++;
		frame_tracker[draw_location] -= 32;
		draw_next_level(0);
	}
}