//http://ce-programming.github.io/toolchain/graphx_8h.html
//https://github.com/CE-Programming/toolchain/wiki/GraphX-Library:-Partial-Redraw

#include "gfx.h"

extern int location_x, square_x, square_y, degree;
extern char level[7][276];

//13 frames at 60fps to do a 90 degree turn
int rotate[4][12][2] = {0};
int position = 0;
int square_pos_old[2] = {0};
bool draw_location = 0; //Zero for Screen, One for Buffer
gfx_sprite_t *behind_draw;
gfx_sprite_t *draw;
gfx_sprite_t *behind_player;
uint8_t background_color;


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
	gfx_FillScreen(background_color);
	gfx_SetTextFGColor(randInt(1, 255));
	gfx_PrintStringXY("Impossible Game CE", (LCD_WIDTH - gfx_GetStringWidth("Impossible Game CE")) / 2, (FONT_HEIGHT) / 2);
	gfx_SwapDraw();
	while (!os_GetCSC()) {} //Wait for any key
	dbg_sprintf(dbgout, "Exiting Main Menu\n");
}

void draw_square(int x, int y, uint8_t color) {//Bottom left corner
	gfx_SetColor(color);
	gfx_FillRectangle_NoClip(x, y - SCALE, SCALE, SCALE);
}

void draw_rectangle(int x, int y, int width, int height, uint8_t color) {//Bottom left corner
	gfx_SetColor(color);
	gfx_FillRectangle_NoClip(x, y - height, width, height);
}

void draw_spike(int x, int y, int width, int height, uint8_t color) {
	gfx_SetColor(color);
	gfx_FillTriangle(x, y - 1, x + (width / 2), y - height, x + width - 1, y - 1);
}

void clear_screen() {
	gfx_FillScreen(background_color);
}

void switch_draw_location() {
	if (draw_location == 0) {
		draw_location = 1;
		gfx_SetDrawBuffer();
	} else {
		draw_location = 0;
		gfx_SetDrawScreen();
	}
}

void draw_fps(int rate) {
	char returnframerate[14];
	sprintf(returnframerate, "  FPS: %d", rate);
	gfx_SetTextFGColor(gfx_black);
	gfx_SetTextBGColor(background_color);
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

	background_color = gfx_RGBTo1555(0x60, 0x60, 0xFF);
	calculate_rotations();
	behind_draw = gfx_MallocSprite(SCALE, LCD_HEIGHT - 16);
	draw = gfx_MallocSprite(SCALE, LCD_HEIGHT - 16);
	behind_player = gfx_MallocSprite(46, 46);
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
	square_pos_old[0] = square_x;
	square_pos_old[1] = square_y;
	gfx_GetSprite(behind_player, square_x - 6, LCD_HEIGHT - 54 - square_y);
	draw_player_rotate(square_x, square_y, degree, gfx_orange);
}

void draw_level(int pixels) {//Screen fits 10 wide, 7 tall, 16px offset from bottom
	int i, j;

	position += pixels;
	gfx_Sprite(behind_player, square_pos_old[0] - 6, LCD_HEIGHT - 54 - square_pos_old[1]);
	gfx_ShiftLeft(pixels);
	square_pos_old[0] = square_x;
	square_pos_old[1] = square_y;
	gfx_GetSprite(behind_player, square_x - 6, LCD_HEIGHT - 54 - square_y);
	draw_player_rotate(square_x, square_y, degree, gfx_orange);

	if (position >= SCALE || location_x == 0) { 
		gfx_Sprite(draw, LCD_WIDTH - position, 0); //Draw the last of what we have
		switch_draw_location(); //Switch to the buffer off screen.
		//gfx_GetSprite(behind_draw, LCD_WIDTH - SCALE, 0); //Grab the background of the offscreen buffer
		gfx_SetColor(background_color);
		gfx_FillRectangle_NoClip(LCD_WIDTH - SCALE, 0, SCALE, LCD_HEIGHT - 16); //Clear old junk
		//gfx_SetColor(gfx_white);
		//gfx_FillRectangle_NoClip(LCD_WIDTH - SCALE, LCD_HEIGHT - HALF_SCALE, LCD_WIDTH, 4); //Base line

		for (i = 1; i <= BOUNDY; i++) {
			for (j = 9; j < BOUNDX; j++) {
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
		gfx_GetSprite(draw, LCD_WIDTH - SCALE, 0); //Put what we just drew into a sprite
		//gfx_Sprite(behind_draw, LCD_WIDTH - SCALE, 0); //Restore what we just overwrote
		switch_draw_location(); //Switch back to the visible screen
		position -= SCALE; 
	}
	
	gfx_Sprite(draw, LCD_WIDTH - position, 0); //Draw new junk

}