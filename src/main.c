//http://higherorderfun.com/blog/2012/05/20/the-guide-to-implementing-2d-platformers/
//https://gamedevelopment.tutsplus.com/tutorials/basic-2d-platformer-physics-part-1--cms-25799

#include "main.h"

uint16_t position[2] = {0};
uint8_t last_move = 0;
//extern uint8_t draw_location;
struct character player = {LCD_HEIGHT - (32 + 16), LCD_HEIGHT - (32 + 16), 0, gfx_orange};

void clock_init() {
	rtc_LoadSeconds = rtc_LoadMinutes = rtc_LoadHours = rtc_LoadDays = 0;
    rtc_Control = RTC_ENABLE | RTC_LOAD | RTC_SEC_INT_SOURCE;
	timer_Control = TIMER1_DISABLE;
	timer_1_Counter = 0;
	timer_Control = TIMER1_ENABLE | TIMER1_32K | TIMER1_UP;

	while (rtc_IsBusy());
    rtc_IntAcknowledge = RTC_INT_MASK;
}

void check_bounds() {
	if (player.degree > 12) player.degree = 0;
	if (player.degree < 0) player.degree = 12;

}

void main() {
    uint8_t move = 0;
	uint8_t tick_rate = 0;
	uint8_t last_tick_rate = 0;
	float tick_time = 0, start_tick = 0;

    init_gfx();
	clock_init();

    while (! kb_Data[6] & kb_Enter) {
		start_tick = (float) atomic_load_increasing_32(&timer_1_Counter);

        kb_Scan();
		if (kb_Data[3] & kb_0) player.degree += 1;
        else if (kb_Data[3] & kb_1) player.degree -= 1;

		move = 352 * tick_time; //Pixels moved per second (32x11)

		check_bounds();

        if (move != 0) {
			draw_next_level(move + last_move); //Draw the new junk
            last_move = move; //Keep frames in sync
        }

		if (rtc_IntStatus & RTC_SEC_INT) {
            draw_fps(tick_rate);
			last_tick_rate = tick_rate;
			tick_rate = 0;
            rtc_IntAcknowledge = rtc_IntStatus;
        } else {
			draw_fps(last_tick_rate);
		}

		swap_draw();

		tick_time = ((float) atomic_load_increasing_32(&timer_1_Counter) - start_tick) / 32768;
		tick_rate++;
        //while (!os_GetCSC()) {}
    }

	exit_gfx();
}