#include "main.h"

// Stats of GeometryJump:
//  * 11 squares per second
//  * Jumps are 4 squares long, 3 squares high
//  * Rotates 90 degrees every 0.21666666 seconds
//  * Full jump time is 0.433333333 seconds

extern char level[7][276]; //The actual level
int attempts = 0; //Unused, for attempt counter
int location_x = 0; //Position of the camera in the x-axis
int location_y = 0; //Currently unused, for the camera
int square_x = SCALE * 2; //Where the square is drawn on screen
int square_y = 0; //Where the square is drawn on screen
bool mainLoop = true; //Maintains the mainloop
bool jumping = false; //Is the character jumping up?
bool show_level = true; //Debugging, do we want to render the level?
int tickrate = 0; //How many frames in a second?
int lastrate = 0; //How many frames were in the last second?
float degree_float = 0; //How far REALLY rotated is the cube?
int degree = 0; //How far rotated is the cube? 0-12

void debug_move() {
	if (kb_Data[7] & kb_Right) {
		dbg_sprintf(dbgout, "Moving Right...\n");
		location_x += SCALE;
	}
	else if (kb_Data[7] & kb_Left) {
		dbg_sprintf(dbgout, "Moving Left...\n");
		location_x -= SCALE;
	}
	else if (kb_Data[7] & kb_Up) {
		dbg_sprintf(dbgout, "Moving Up...\n");
		square_y += SCALE;
	}
	else if (kb_Data[7] & kb_Down) {
		dbg_sprintf(dbgout, "Moving Down...\n");
		square_y -= SCALE;
	}
	else if (kb_Data[6] & kb_Add && degree < 12) degree += 1;
	else if (kb_Data[6] & kb_Sub && degree > 0) degree -= 1;
}

void move(float time, int moved) {
	location_x += moved; //Move right
	//dbg_sprintf(dbgout, "Location X: %d\n", location_x);
	if (location_x > LEVEL_LENGTH) location_x = 0; //Stop from crossing into RAM

	if (kb_Data[1] & kb_2nd && square_y == 0) {//Starting Jump
			dbg_sprintf(dbgout, "Jumping...\n");
			jumping = true;
	}

	if (!jumping && square_y > 0) { //Falling
		square_y -= SCALE * 3 * time / (0.433333 / 2);
		if (square_y < 0) square_y = 0;
		//dbg_sprintf(dbgout, "Location Y: %d\n", square_y);
	}

	if (jumping && square_y < SCALE * 3) { //Jumping
		square_y += SCALE * 3 * time / (0.433333 / 2);
		if (square_y >= 96) {
			square_y = 96;
			dbg_sprintf(dbgout, "Falling...\n");
			jumping = false;
		} 
		//dbg_sprintf(dbgout, "Location Y: %d\n", square_y);
	}

	if (jumping || square_y > 0 || degree_float != 0) {
		degree_float += 13 * time / 0.216666;
		if (degree_float > 26) degree_float = 0;
		degree = (int)degree_float % 13;
		dbg_sprintf(dbgout, "Rotation Degree: %d, Float: %d\n", degree, (int)degree_float);
	}
}

void check_bounds() {
	if (location_x + LCD_WIDTH > LEVEL_LENGTH) {
		location_x = LEVEL_LENGTH - LCD_WIDTH;
	}
	if (location_x < 0) location_x = 0;
	if (square_y > LCD_HEIGHT) square_y = LCD_HEIGHT;
	if (square_y < 0) square_y = 0;
}

void clock_init() {
	rtc_LoadSeconds = rtc_LoadMinutes = rtc_LoadHours = rtc_LoadDays = 0;
    rtc_Control = RTC_ENABLE | RTC_LOAD | RTC_SEC_INT_SOURCE;
	timer_Control = TIMER1_DISABLE;
	timer_1_Counter = 0;
	timer_Control = TIMER1_ENABLE | TIMER1_32K | TIMER1_UP;

	while (rtc_IsBusy());
    rtc_IntAcknowledge = RTC_INT_MASK;
}

void main() {
	float tick_time = 0, start_tick = 0;
	int moved;

	gfx_Begin();
	//draw_main_menu();

	calculate_rotations();
	clock_init();
	gfx_SetDrawBuffer();
	init_level();
	gfx_BlitBuffer();
	while (mainLoop) {
		start_tick = (float)atomic_load_increasing_32(&timer_1_Counter) / 32768;
		//clear_screen();
		kb_Scan();

		if (kb_Data[6] & kb_Enter) {
			dbg_sprintf(dbgout, "Breaking...\n");
			break;
		} else if (kb_Data[1] & kb_Zoom) {
			show_level = false;
		} else if (kb_Data[1] & kb_Trace) {
			show_level = true;
		}
		moved = (SCALE * 11) * tick_time;

		//draw_player_rotate(square_x, square_y, degree, gfx_orange); //Player should actually be drawn last probably
		
		check_bounds();
		debug_move();
		move(tick_time, moved);
		gfx_SetDrawBuffer();
		if (show_level) draw_level(moved);
		gfx_SetDrawScreen();
		gfx_ShiftLeft(moved);
		dbg_sprintf(dbgout, "From %i to %i\n", LCD_WIDTH - moved, moved);
		if (moved > 0) gfx_BlitRectangle(gfx_buffer, LCD_WIDTH - moved, 0, moved, LCD_HEIGHT);

		if (rtc_IntStatus & RTC_SEC_INT) {
            draw_fps(tickrate);
			lastrate = tickrate;
			tickrate = 0;
            rtc_IntAcknowledge = rtc_IntStatus;
        } else {
			draw_fps(lastrate);
		}
		gfx_SetDrawBuffer();
		//gfx_Wait();
		//gfx_SwapDraw();
		//gfx_BlitBuffer();
		tick_time = ((float)atomic_load_increasing_32(&timer_1_Counter) / 32768) - start_tick;
		tickrate++;
		//while (!os_GetCSC()) {}
	}
	gfx_End();
}
