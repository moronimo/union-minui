// loosely based on https://github.com/gameblabla/clock_sdl_app

#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "defines.h"
#include "utils.h"
#include "api.h"

int main(int argc , char* argv[]) {
	SDL_Surface* screen = GFX_init();
	
	// TODO: make use of SCALE1()
	SDL_Surface* digits = SDL_CreateRGBSurface(SDL_SWSURFACE, 240,32, 16, 0,0,0,0);
	SDL_FillRect(digits, NULL, RGB_BLACK);
	
	SDL_Surface* digit;
	char* chars[] = { "0","1","2","3","4","5","6","7","8","9","/",":", NULL };
	char* c;
	int i = 0;
#define DIGIT_WIDTH 20
#define DIGIT_HEIGHT 32
	while (c = chars[i]) {
		digit = TTF_RenderUTF8_Blended(font.large, c, COLOR_WHITE);
		int y = i==11 ? -3 : 0; // : sits too low naturally
		SDL_BlitSurface(digit, NULL, digits, &(SDL_Rect){ (i * DIGIT_WIDTH) + (DIGIT_WIDTH - digit->w)/2, y + (DIGIT_HEIGHT - digit->h)/2});
		SDL_FreeSurface(digit);
		i += 1;
	}
	
	SDL_Event event;
	int quit = 0;
	int save_changes = 0;
	int select_cursor = 0;
	int show_24hour = 0; // TODO: exists(USERDATA_PATH "/show_24hour");
	
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	
	int32_t day_selected = tm.tm_mday;
	int32_t month_selected = tm.tm_mon + 1;
	uint32_t year_selected = tm.tm_year + 1900;
	int32_t hour_selected = tm.tm_hour;
	int32_t minute_selected = tm.tm_min;
	int32_t seconds_selected = tm.tm_sec;
	int am_selected = hour_selected < 12;

	#define kSlash 10
	#define kColon 11
	#define kBar 12
	int blit(int i, int x, int y) {
		SDL_BlitSurface(digits, &(SDL_Rect){i*20,0,20,32}, screen, &(SDL_Rect){x,y});
		return x + 20;
	}
	void blitBar(int x, int y, int w) {
		GFX_blitPill(ASSET_UNDERLINE, screen, &(SDL_Rect){ x,y,w});
	}
	int blitNumber(int num, int x, int y) {
		int n;
		if (num > 999) {
			n = num / 1000;
			num -= n * 1000;
			x = blit(n, x,y);
			
			n = num / 100;
			num -= n * 100;
			x = blit(n, x,y);
		}
		n = num / 10;
		num -= n * 10;
		x = blit(n, x,y);
		
		n = num;
		x = blit(n, x,y);
		
		return x;
	}
	void validate(void) {
		// leap year
		uint32_t february_days = 28;
		if ( ((year_selected % 4 == 0) && (year_selected % 100 != 0)) || (year_selected % 400 == 0)) february_days = 29;
	
		// day
		// if (day_selected < 1) day_selected = 1;
		if (month_selected > 12) month_selected -= 12;
		else if (month_selected < 1) month_selected += 12;
	
		if (year_selected > 2100) year_selected = 2100;
		else if (year_selected < 1970) year_selected = 1970;
	
		switch(month_selected)
		{
			case 2:
				if (day_selected > february_days) day_selected -= february_days;
				else if (day_selected<1) day_selected += february_days;
				break;
			case 4:
			case 6:
			case 9:
			case 11:
				if (day_selected > 30) day_selected -= 30;
				else if (day_selected < 1) day_selected += 30;

				break;
			default:
				if (day_selected > 31) day_selected -= 31;
				else if (day_selected < 1) day_selected += 31;
				break;
		}
	
		// time
		if (hour_selected > 23) hour_selected -= 24;
		else if (hour_selected < 0) hour_selected += 24;
		if (minute_selected > 59) minute_selected -= 60;
		else if (minute_selected < 0) minute_selected += 60;
		if (seconds_selected > 59) seconds_selected -= 60;
		else if (seconds_selected < 0) seconds_selected += 60;
	}
	
	int option_count = 7;
	
	int dirty = 1;
	while(!quit) {
		unsigned long frame_start = SDL_GetTicks();
		
		PAD_poll();
		
		if (PAD_justRepeated(BTN_UP)) {
			dirty = 1;
			switch(select_cursor) {
				case 0:
					year_selected++;
				break;
				case 1:
					month_selected++;
				break;
				case 2:
					day_selected++;
				break;
				case 3:
					hour_selected++;
				break;
				case 4:
					minute_selected++;
				break;
				case 5:
					seconds_selected++;
				break;
				case 6:
					hour_selected += 12;
				break;
			}
		}
		else if (PAD_justRepeated(BTN_DOWN)) {
			dirty = 1;
			switch(select_cursor) {
				case 0:
					year_selected--;
				break;
				case 1:
					month_selected--;
				break;
				case 2:
					day_selected--;
				break;
				case 3:
					hour_selected--;
				break;
				case 4:
					minute_selected--;
				break;
				case 5:
					seconds_selected--;
				break;
				case 6:
					hour_selected -= 12;
				break;
				
			}
		}
		else if (PAD_justRepeated(BTN_LEFT)) {
			dirty = 1;
			select_cursor--;
			if (select_cursor < 0) select_cursor += option_count;
		}
		else if (PAD_justRepeated(BTN_RIGHT)) {
			dirty = 1;
			select_cursor++;
			if (select_cursor >= option_count) select_cursor -= option_count;
		}
		else if (PAD_justPressed(BTN_A)) {
			save_changes = 1;
			quit = 1;
		}
		else if (PAD_justPressed(BTN_B)) {
			quit = 1;
		}
		else if (PAD_justPressed(BTN_SELECT)) {
			dirty = 1;
			show_24hour = !show_24hour;
			option_count = show_24hour ? 6 : 7;
			if (select_cursor >= option_count) select_cursor -= option_count;
		}
		
		if (dirty) {
			dirty = 0;
			
			validate();
		
			// render
			GFX_clear(screen);
			GFX_blitButtonGroup((char*[]){ "SELECT",show_24hour?"12 HOUR":"24 HOUR", NULL }, screen, 0);
			GFX_blitButtonGroup((char*[]){ "B","CANCEL", "A","SET", NULL }, screen, 1);
		
			// datetime
			int x = show_24hour ? 130 : 90;
			int y = 185;
		
			x = blitNumber(year_selected, x,y);
			x = blit(kSlash, x,y);
			x = blitNumber(month_selected, x,y);
			x = blit(kSlash, x,y);
			x = blitNumber(day_selected, x,y);
			x += 20; // space
			
			am_selected = hour_selected < 12;
			if (show_24hour) {
				 x = blitNumber(hour_selected, x,y);
			}
			else {
				// if (select_cursor==3) blitNumber(hour_selected, x,233);
				
				// 12 hour
				int hour = hour_selected;
				if (hour==0) hour = 12;
				else if (hour>12) hour -= 12;
				x = blitNumber(hour, x,y);
			}
			x = blit(kColon, x,y);
			x = blitNumber(minute_selected, x,y);
			x = blit(kColon, x,y);
			x = blitNumber(seconds_selected, x,y);
			
			int ampm_w;
			if (!show_24hour) {
				x += 20; // space
				SDL_Surface* text = TTF_RenderUTF8_Blended(font.large, am_selected ? "AM" : "PM", COLOR_WHITE);
				ampm_w = text->w;
				SDL_BlitSurface(text, NULL, screen, &(SDL_Rect){x,y-6});
				SDL_FreeSurface(text);
			}
		
			// cursor
			x = show_24hour ? 130 : 90;
			y = 222;
			if (select_cursor>0) {
				x += 100; // YYYY/
				x += (select_cursor - 1) * 60;
			}
			blitBar(x,y, (select_cursor==0 ? 80 : (select_cursor==6 ? ampm_w : 40)));
		
			GFX_flip(screen);
		}
		else {
			// slow down to 60fps
			unsigned long frame_duration = SDL_GetTicks() - frame_start;
			#define kTargetFrameDuration 17
			if (frame_duration<kTargetFrameDuration) SDL_Delay(kTargetFrameDuration-frame_duration);
		}
	}
	
	SDL_FreeSurface(digits);

	GFX_quit();
	
	if (save_changes) {
		char cmd[512];
		snprintf(cmd, sizeof(cmd), "date -u -s '%d-%d-%d %d:%d:%d';hwclock --utc -w", year_selected, month_selected, day_selected, hour_selected, minute_selected, seconds_selected);
		system(cmd);
	}
	
	return EXIT_SUCCESS;
}