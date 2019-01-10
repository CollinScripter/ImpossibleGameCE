//http://ce-programming.github.io/toolchain/graphx_8h.html
//https://github.com/CE-Programming/toolchain/wiki/GraphX-Library:-Partial-Redraw

#include "gfx.h"

extern int location_x;
extern char level[7][276];

//13 frames at 60fps to do a 90 degree turn
int rotate[4][12][2] = {0};
int position = 0;

void calculate_rotations() {
	int point[2] = {0};
	int i, j;
	float angle, s, c;
	for (i = 0; i < 4; i++) {
		switch(i) {
			case 0: //Top Left
				point[0] = -HALF_SCALE;
				point[1] = -HALF_SCALE;
				break;
			case 1: //Top Right
				point[0] = HALF_SCALE;
				point[1] = -HALF_SCALE;
				break;
			case 2: //Bottom Left
				point[0] = -HALF_SCALE;
				point[1] = HALF_SCALE;
				break;
			case 3: //Bottom Right
				point[0] = HALF_SCALE;
				point[1] = HALF_SCALE;
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

void draw_main_menu() {
	//Key? Red? Light Blue?
	dbg_sprintf(dbgout, "Entering Main Menu\n");
	gfx_FillScreen(gfx_RGBTo1555(0x60, 0x60, 0xFF));
	gfx_SetTextFGColor(randInt(1, 255));
	gfx_PrintStringXY("Impossible Game CE", (LCD_WIDTH - gfx_GetStringWidth("Impossible Game CE")) / 2, (FONT_HEIGHT) / 2);
	gfx_SwapDraw();
	while (!os_GetCSC()) {} //Wait for any key
	dbg_sprintf(dbgout, "Exiting Main Menu\n");
}

void draw_square(int x, int y, uint8_t color) {//Bottom left corner
	gfx_SetColor(color);
	gfx_FillRectangle(x, y - SCALE, SCALE, SCALE);
}

void draw_rectangle(int x, int y, int width, int height, uint8_t color) {//Bottom left corner
	gfx_SetColor(color);
	gfx_FillRectangle(x, y - height, width, height);
}

void draw_spike(int x, int y, int width, int height, uint8_t color) {
	gfx_SetColor(color);
	gfx_FillTriangle(x, y - 1, x + (width / 2), y - height, x + width - 1, y - 1);
}

void clear_screen() {
	gfx_FillScreen(gfx_RGBTo1555(0x60, 0x60, 0xFF));
}

void draw_fps(int rate) {
	char returnframerate[12];
	sprintf(returnframerate, "FPS: %d", rate);
	gfx_SetTextFGColor(gfx_black);
	gfx_SetTextBGColor(gfx_RGBTo1555(0x60, 0x60, 0xFF));
	gfx_PrintStringXY(returnframerate, 0, (FONT_HEIGHT) / 2);
}

void draw_player_rotate(int x0, int y0, int angle, uint8_t color) {
	int y = LCD_HEIGHT - y0 - (SCALE * 1);
	int x = x0 + HALF_SCALE;
	int angle_frame = angle - 1;
	gfx_SetColor(color);
	if (angle == 0) {
		gfx_FillRectangle(x - HALF_SCALE, y - HALF_SCALE, SCALE, SCALE);
	} else {
		gfx_FillTriangle(rotate[0][angle_frame][0] + x, rotate[0][angle_frame][1] + y, rotate[1][angle_frame][0] + x, rotate[1][angle_frame][1] + y, rotate[2][angle_frame][0] + x, rotate[2][angle_frame][1] + y);
		gfx_FillTriangle(rotate[1][angle_frame][0] + x, rotate[1][angle_frame][1] + y, rotate[2][angle_frame][0] + x, rotate[2][angle_frame][1] + y, rotate[3][angle_frame][0] + x, rotate[3][angle_frame][1] + y);
		//dbg_sprintf(dbgout, "Y0: %d Y1: %d Y2: %d Y3: %d\n", rotate[0][angle_frame][1] + y, rotate[1][angle_frame][1] + y, rotate[2][angle_frame][1] + y, rotate[3][angle_frame][1] + y);
		//dbg_sprintf(dbgout, "X0: %d X1: %d X2: %d X3: %d\n", rotate[0][angle_frame][0] + x, rotate[1][angle_frame][0] + x, rotate[2][angle_frame][0] + x, rotate[3][angle_frame][0] + x);
	}
}

void init_level() {//Screen fits 10 wide, 7 tall, 16px offset from bottom
	int i, j;

	calculate_rotations();
	clear_screen();

	gfx_SetColor(gfx_white);
	gfx_FillRectangle(0, LCD_HEIGHT - HALF_SCALE, LCD_WIDTH, 4); //Base line

	for (i = 1; i <= BOUNDY; i++) {
		for (j = 0; j < BOUNDX; j++) {
			switch (level[i - 1][j + (location_x / SCALE)]) {
				case 'D': //Square Block Cross
				case 'B': //Square Block
					draw_square(j * SCALE, i * SCALE, gfx_blue);
					break;
				case 'x': //Small Spike
					draw_spike(j * SCALE, i * SCALE, SCALE, HALF_SCALE, gfx_red);
					break;
				case 'X': //Large Spike
					draw_spike(j * SCALE, i * SCALE, SCALE, SCALE, gfx_red);
					break;
				case 's': //Two Small Spikes
					draw_spike(j * SCALE, i * SCALE, HALF_SCALE, HALF_SCALE, gfx_red);
					draw_spike(j * SCALE + HALF_SCALE, i * SCALE, HALF_SCALE, HALF_SCALE, gfx_red);
					break;
				case '-': //Top Half Block
					draw_rectangle(j * SCALE, i * SCALE - 16, SCALE, HALF_SCALE, gfx_blue);
					break;
				case 'F': //Finish Line
					draw_square(j * SCALE, i * SCALE, gfx_yellow);
					break;
			}
		}
	}
}

void draw_level(int pixels) {//Screen fits 10 wide, 7 tall, 16px offset from bottom
	int i, j;

	position += pixels;

	gfx_SetColor(gfx_white);
	gfx_FillRectangle(0, LCD_HEIGHT - HALF_SCALE, LCD_WIDTH, 4); //Base line

	for (i = 1; i <= BOUNDY; i++) {
		for (j = 0; j < BOUNDX; j++) {
			switch (level[i - 1][j + (location_x / SCALE)]) {
				case 'D': //Square Block Cross
				case 'B': //Square Block
					draw_square(j * SCALE, i * SCALE, gfx_blue);
					break;
				case 'x': //Small Spike
					draw_spike(j * SCALE, i * SCALE, SCALE, HALF_SCALE, gfx_red);
					break;
				case 'X': //Large Spike
					draw_spike(j * SCALE, i * SCALE, SCALE, SCALE, gfx_red);
					break;
				case 's': //Two Small Spikes
					draw_spike(j * SCALE, i * SCALE, HALF_SCALE, HALF_SCALE, gfx_red);
					draw_spike(j * SCALE + HALF_SCALE, i * SCALE, HALF_SCALE, HALF_SCALE, gfx_red);
					break;
				case '-': //Top Half Block
					draw_rectangle(j * SCALE, i * SCALE - 16, SCALE, HALF_SCALE, gfx_blue);
					break;
				case 'F': //Finish Line
					draw_square(j * SCALE, i * SCALE, gfx_yellow);
					break;
			}
		}
	}
}