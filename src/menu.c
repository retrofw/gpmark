#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include "font.h"
#include "menu.h"
#include "main.h"
#include <signal.h>

#ifndef SDL_TRIPLEBUF
#define SDL_TRIPLEBUF SDL_DOUBLEBUF
#endif

#define WIDTH  ScreenWidth
#define HEIGHT ScreenHeight

#define BTN_X			SDLK_SPACE
#define BTN_A			SDLK_LCTRL
#define BTN_B			SDLK_LALT
#define BTN_Y			SDLK_LSHIFT
#define BTN_L			SDLK_TAB
#define BTN_R			SDLK_BACKSPACE
#define BTN_START		SDLK_RETURN
#define BTN_SELECT		SDLK_ESCAPE
#define BTN_BACKLIGHT	SDLK_3
#define BTN_POWER		SDLK_END
#define BTN_UP			SDLK_UP
#define BTN_DOWN		SDLK_DOWN
#define BTN_LEFT		SDLK_LEFT
#define BTN_RIGHT		SDLK_RIGHT

TTF_Font *font = NULL;
extern SDL_Surface *screen;// = NULL;
// SDL_Surface *StretchSurface = NULL;
SDL_Surface *bg = NULL;
SDL_Event event;

uint8_t *keys;

SDL_Color txtColor = {200, 200, 220};
SDL_Color titleColor = {200, 200, 0};
SDL_Color subTitleColor = {0, 200, 0};
SDL_Color powerColor = {200, 0, 0};

static char buf[1024];
uint8_t nextline = 24;

int memdev = -1;
uint32_t *mem;
int selected = 0;

enum modes {
	MODE_UNKNOWN,
	MODE_UDC,
	MODE_NETWORK,
	MODE_RESIZE,
	MODE_FSCK,
	MODE_DEFL,
	MODE_MKSWAP,
	MODE_START,
	MODE_MENU
};

struct callback_map_t {
  const char *text;
  const char *value;
  int ivalue;
  int (*callback)(int);
};

void quit(int err) {
	font = NULL;
	SDL_Quit();
	TTF_Quit();
	exit(err);
}

int draw_text(int x, int y, const char buf[64], SDL_Color txtColor) {
	if (!strcmp(buf, "")) return y;
	SDL_Surface *msg = TTF_RenderText_Blended(font, buf, txtColor);
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = msg->w;
	rect.h = msg->h;
	SDL_BlitSurface(msg, NULL, screen, &rect);
	SDL_FreeSurface(msg);
	return y + msg->h + 2;
}

int draw_screen(const char title[64], const char footer[64]) {
	SDL_Rect rect;
	rect.w = WIDTH;
	rect.h = HEIGHT;
	rect.x = 0;
	rect.y = 0;

	SDL_FillRect(screen, NULL,  SDL_MapRGB(screen->format, 0, 0, 0));

	// title
	draw_text(247, 4, "RetroFW", titleColor);
	draw_text(10, 4, title, titleColor);

	rect.w = WIDTH - 20;
	rect.h = 1;
	rect.x = 10;
	rect.y = 20;
	SDL_FillRect(screen, &rect,  SDL_MapRGB(screen->format, 200, 200, 200));

	rect.y = HEIGHT - 20;
	SDL_FillRect(screen, &rect,  SDL_MapRGB(screen->format, 200, 200, 200));

	draw_text(10, 222, footer, powerColor);

	return 32;
}

struct callback_map_t cb_map[] = {
  { "Surface", "HARDWARE", 1, surface },
  { "Buffers", "SINGLE", 1, buffer },
  { "Run", "", 0, run },
};
unsigned int cb_size = (sizeof(cb_map) / sizeof(cb_map[0]));

int buffer(int action) {
	switch (action) {
		case BTN_RIGHT:
			cb_map[selected].ivalue++;
			break;
		case BTN_LEFT:
			cb_map[selected].ivalue--;
			break;
	}

	if (cb_map[selected].ivalue == 2) {
		cb_map[selected].value = "DOUBLE";
	} else
	if (cb_map[selected].ivalue == 3 || cb_map[selected].ivalue < 1) {
		cb_map[selected].ivalue = 3;
		cb_map[selected].value = "TRIPLE";
	} else
	if (cb_map[selected].ivalue == 1 || cb_map[selected].ivalue > 3) {
		cb_map[selected].ivalue = 1;
		cb_map[selected].value = "SINGLE";
	}

	return 0;
}

int surface(int action) {
	switch (action) {
		case BTN_RIGHT:
			cb_map[selected].ivalue++;
			break;
		case BTN_LEFT:
			cb_map[selected].ivalue--;
			break;
	}

	if (cb_map[selected].ivalue == 1 || cb_map[selected].ivalue < 0) {
		cb_map[selected].ivalue = 1;
		cb_map[selected].value = "HARDWARE";
	} else
	if (cb_map[selected].ivalue == 0 || cb_map[selected].ivalue > 1) {
		cb_map[selected].ivalue = 0;
		cb_map[selected].value = "SOFTWARE";
	}

	return 0;
}

int run(int action) {
	int flags;

	if (cb_map[0].ivalue == 0) {
		flags = SDL_SWSURFACE;
	} else {
		flags = SDL_HWSURFACE;
	}

	if (cb_map[1].ivalue == 2) {
		flags |= SDL_DOUBLEBUF;
	} else
	if (cb_map[1].ivalue == 3) {
		flags |= SDL_TRIPLEBUF;
	}

    screen = SDL_SetVideoMode(WIDTH, HEIGHT, 16, flags);

	return (action == BTN_A);
}

int menu() {
	keys = SDL_GetKeyState(NULL);

	char title[64] = "GPMARK";

	SDL_Rect rect = {0};

	SDL_PumpEvents();
	SDL_EnableKeyRepeat(0, 0);
	SDL_Delay(50);
	SDL_PumpEvents();

	if (TTF_Init() == -1) {
		printf("TTF_Init: %s\n", SDL_GetError());
		return -1;
	}

	font = TTF_OpenFontRW(SDL_RWFromMem(rwfont, sizeof(rwfont)), 1, 12);
	TTF_SetFontHinting(font, TTF_HINTING_NORMAL);
	TTF_SetFontOutline(font, 0);

	int ret = 0;
	while (!ret) {
		nextline = draw_screen(title, "<->: TOGGLE             A: SELECT");

		for (int i = 0; i < cb_size; i++) {
			SDL_Color selColor = txtColor;
			if (selected == i) selColor = subTitleColor;
			draw_text(10, nextline, cb_map[i].text, selColor);
			nextline = draw_text(230, nextline, cb_map[i].value, selColor);
		}

		SDL_Flip(screen);

		if (SDL_WaitEvent(&event)) {
			SDL_PumpEvents();

			if (event.type == SDL_KEYDOWN) {
				if (keys[BTN_UP]) {
					selected--;
					if (selected < 0) selected = cb_size - 1;
				} else if (keys[BTN_DOWN]) {
					selected++;
					if (selected >= cb_size) selected = 0;
				} else if (keys[BTN_LEFT] || keys[BTN_RIGHT] || keys[BTN_A]) {
					ret = cb_map[selected].callback(event.key.keysym.sym);
				}
			}
		}
	}

	return ret;
}
