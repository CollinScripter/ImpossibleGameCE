//http://ce-programming.github.io/toolchain/graphx_8h.html
//https://github.com/CE-Programming/toolchain/wiki/GraphX-Library:-Partial-Redraw

#include "gfx.h"

int rotate[4][12][2] = {0};
uint8_t background_color;
uint8_t frame_tracker[2];
uint8_t draw_location = 0; //0 is screen, 1 is buffer

extern char level[7][276];
extern uint16_t position[2];
extern struct character player;

//Sprites
gfx_sprite_t *square_tile;
gfx_sprite_t *top_half_tile;
gfx_sprite_t *double_spike_tile;
gfx_sprite_t *large_spike_tile;
gfx_sprite_t *small_spike_tile;
gfx_sprite_t *finish_tile;
gfx_sprite_t *behind_player[2];
gfx_sprite_t *player_rotations[12];

void init_gfx() {
	uint8_t i;

	background_color = gfx_RGBTo1555(0x60, 0x60, 0xFF);
	behind_player[0] = gfx_MallocSprite(46, 46);
	behind_player[1] = gfx_MallocSprite(46, 46);

	for (i = 0; i < 12; i++) {
		player_rotations[i] = gfx_MallocSprite(46, 46);
	}

	gfx_Begin();
	gfx_SetDrawBuffer();
	gfx_SetTransparentColor(254);
	calculate_rotations();
	init_tiles();
	clear_screen();
	draw_start_level();
	gfx_GetSprite(behind_player[0], PLAYER_X - 7, player.y - 7);
	gfx_GetSprite(behind_player[1], PLAYER_X - 7, player.y - 7);
	gfx_BlitBuffer();
}

void exit_gfx() {
	uint8_t i;

	free(square_tile);
	free(top_half_tile);
	free(double_spike_tile);
	free(large_spike_tile);
	free(small_spike_tile);
	free(finish_tile);
	free(behind_player[0]);
	free(behind_player[1]);
	for (i = 0; i < 12; i++) {
		free(player_rotations[i]);
	}

	gfx_End();
}

void init_tiles() {
	clear_screen();

	square_tile = gfx_MallocSprite(32, 32);
	top_half_tile = gfx_MallocSprite(32, 16);
	double_spike_tile = gfx_MallocSprite(32, 16);
	large_spike_tile = gfx_MallocSprite(32, 32);
	small_spike_tile = gfx_MallocSprite(32, 16);
	finish_tile = gfx_MallocSprite(32, 32);

	gfx_SetColor(gfx_blue);
	gfx_FillRectangle(0, 0, 32, 32); //Square Tile
	gfx_FillRectangle(32, 0, 32, 16); //Top Half Tile

	gfx_SetColor(gfx_red);
	gfx_FillTriangle(64, 16 - 1, 64 + 16 / 2, 0, 64 + 16 - 1, 16 - 1); //Double Spike #1
	gfx_FillTriangle(80, 16 - 1, 80 + 16 / 2, 0, 80 + 16 - 1, 16 - 1); //Double Spike #2
	gfx_FillTriangle(96, 32 - 1, 96 + 32 / 2, 0, 96 + 32 - 1, 32 - 1); //Large Spike
	gfx_FillTriangle(128, 16 - 1, 128 + 32 / 2, 0, 128 + 32 - 1, 16 - 1); //Small Spike

	gfx_SetColor(gfx_yellow);
	gfx_FillRectangle(160, 0, 32, 32); //Finish Tile

	gfx_GetSprite(square_tile, 0, 0);
	gfx_GetSprite(top_half_tile, 32, 0);
	gfx_GetSprite(double_spike_tile, 64, 0);
	gfx_GetSprite(large_spike_tile, 96, 0);
	gfx_GetSprite(small_spike_tile, 128, 0);
	gfx_GetSprite(finish_tile, 160, 0);
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
		switch (i) {
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

	for (i = 0; i < 12; i++) { //23 = 16 + 7
		gfx_SetColor(254);
		gfx_FillRectangle(0, 0, 46, 46);
		gfx_SetColor(player.color);
		gfx_FillTriangle(rotate[0][i][0] + 23, rotate[0][i][1] + 23, rotate[1][i][0] + 23, rotate[1][i][1] + 23, rotate[2][i][0] + 23, rotate[2][i][1] + 23);
		gfx_FillTriangle(rotate[1][i][0] + 23, rotate[1][i][1] + 23, rotate[2][i][0] + 23, rotate[2][i][1] + 23, rotate[3][i][0] + 23, rotate[3][i][1] + 23);
		gfx_GetSprite(player_rotations[i], 0, 0);

	}
}

void clear_screen() {
	gfx_FillScreen(background_color);
}

void draw_start_level() { //Draws the initial stage area
    uint8_t x, y;

    for (y = 0; y < 7; y++) {
		for (x = 0; x < 10; x++) {
			switch (level[y][x]) {
				case 'D': //Square Block
					gfx_Sprite(square_tile, x * 32, y * 32);
					break;
				case 's': //Two Small Spikes
					gfx_Sprite(double_spike_tile, x * 32, y * 32 + 16);
					break;
				case 'X': //Large Spike
					gfx_Sprite(large_spike_tile, x * 32, y * 32);
					break;
				case 'x': //Small Spike
					gfx_Sprite(small_spike_tile, x * 32, y * 32 + 16);
					break;
				case '-': //Top Half Block
					gfx_Sprite(top_half_tile, x * 32, y * 32);
					break;
				case 'F': //Finish Line
					gfx_Sprite(finish_tile, x * 32, y * 32);
					break;
			}
		}
	}

	gfx_SetColor(gfx_white);
	gfx_FillRectangle_NoClip(0, LCD_HEIGHT - 16, LCD_WIDTH, 4);
}

void draw_fps(int rate) {
	char returnframerate[8];
	sprintf(returnframerate, "FPS: %d", rate);
	gfx_SetTextFGColor(gfx_black);
	gfx_SetTextBGColor(background_color);
	gfx_PrintStringXY(returnframerate, 0, 4);
}

void draw_next_level(uint8_t movement) { //Draws the next stage area
    uint8_t y = 7;

    if (movement) {
		gfx_Sprite_NoClip(behind_player[draw_location], PLAYER_X - 7, player.prev_y - 7); //Restore area behind player
		
		gfx_ShiftLeft(movement); //Move left
		
		gfx_GetSprite_NoClip(behind_player[draw_location], PLAYER_X - 7, player.y - 7);

		if (player.degree) {
			gfx_TransparentSprite_NoClip(player_rotations[player.degree - 1], PLAYER_X - 7, player.y - 7);
		} else {
			gfx_SetColor(player.color);
			gfx_FillRectangle_NoClip(PLAYER_X, player.y, 32, 32);
		}

		player.prev_y = player.y;

		gfx_SetColor(background_color);
		gfx_FillRectangle_NoClip(LCD_WIDTH - movement, 0, movement, 224); //Blank out previous drawing area
		gfx_SetColor(gfx_white);
		gfx_FillRectangle_NoClip(LCD_WIDTH - movement, LCD_HEIGHT - 16, movement, 4); //Draw line
		frame_tracker[draw_location] += movement;
	}

    while (y--) {
		switch (level[y][10 + position[draw_location]]) {
			case 'D': //Square Block
				gfx_SetColor(gfx_blue);
				gfx_FillRectangle(320 - frame_tracker[draw_location], y * 32, 32, 32);
				break;
			case 's': //Two Small Spikes
				gfx_Sprite(double_spike_tile, 320 - frame_tracker[draw_location], y * 32 + 16);
				break;
			case 'X': //Large Spike
				gfx_Sprite(large_spike_tile, 320 - frame_tracker[draw_location], y * 32);
				break;
			case 'x': //Small Spike
				gfx_Sprite(small_spike_tile, 320 - frame_tracker[draw_location], y * 32 + 16);
				break;
			case '-': //Top Half Block
				gfx_Sprite(top_half_tile, 320 - frame_tracker[draw_location], y * 32);
				break;
			case 'F': //Finish Line
				gfx_Sprite(finish_tile, 320 - frame_tracker[draw_location], y * 32);
				break;
		}
	}

	if (frame_tracker[draw_location] >= 32) {
		position[draw_location]++;
		frame_tracker[draw_location] -= 32;
		draw_next_level(0);
	}
}