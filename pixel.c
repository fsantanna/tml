#if 0
#!/bin/sh
gcc -Wall -I ../pico-sdl/src/ `sdl2-config --cflags` tml.c ../pico-sdl/src/pico.c pixel.c -o xpixel `sdl2-config --libs` -lSDL2_image -lSDL2_ttf
exit
#endif

#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>
#include "tml.h"

enum {
    TML_EVT_JUMP = TML_EVT_NEXT
};

void cb_sim (tml_evt);
void cb_eff (int trv);
int  cb_rec (tml_evt* evt);
int  cb_trv (int max, int cur, int* ret);

#define FPS   50
#define WIN   400
#define FLOOR (2*WIN/3)
#define LAKEW 60
#define LAKEX (WIN/2-LAKEW/2)

#define DX    3
#define DYG   5
#define DYJ   -10
#define DIM   10

struct {
    int x, y;
    int dy;
    int dead;
} G;

SDL_Renderer* REN = NULL;

int main (void) {
    assert(SDL_Init(SDL_INIT_VIDEO) == 0);

    SDL_Window* win = SDL_CreateWindow (
        "TML: Time Machine Library",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        WIN, WIN,
        SDL_WINDOW_SHOWN
    );
    assert(win != NULL);

    REN = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    assert(REN != NULL);
    SDL_SetRenderDrawBlendMode(REN,SDL_BLENDMODE_BLEND);

    tml_loop(50, sizeof(G), &G, cb_sim, cb_eff, cb_rec, cb_trv);

    SDL_DestroyRenderer(REN);
    SDL_DestroyWindow(win);
    SDL_Quit();
}

void cb_sim (tml_evt evt) {
    switch (evt.id) {
        case TML_EVT_INIT:
            G.x    = 0;
            G.y    = FLOOR;
            G.dy   = DYG;
            G.dead = 0;
            break;
        case TML_EVT_TICK:
            if (!G.dead) {
                G.x  = (G.x + DX) % WIN;
                G.y  = MIN(FLOOR, G.y+G.dy);
                G.dy = MIN(DYG, G.dy+1);
                if ((G.y == FLOOR) && (LAKEX-DIM/2<=G.x && G.x<=LAKEX+LAKEW-DIM/2)) {
                    G.dead = evt.pay.tick;
                }
            }
            break;
        case TML_EVT_JUMP:
            if (!G.dead) {
                if (G.y == FLOOR) {
                    G.dy = DYJ;
                }
            }
            break;
    }
}

void cb_eff (int trv) {
    SDL_SetRenderDrawColor(REN, 0xFF,0xFF,0xFF,0xFF);
    SDL_RenderClear(REN);
    {
        // FLOOR
        SDL_Rect r = { 0, FLOOR+DIM, WIN, 4*DIM };
        SDL_SetRenderDrawColor(REN, 0x4B,0x37,0x1C,0xFF);
        SDL_RenderFillRect(REN, &r);
    }
    {
        // LAKE
        SDL_Rect r = { LAKEX, FLOOR+DIM, LAKEW, 2*DIM };
        SDL_SetRenderDrawColor(REN, 0x00,0x00,0xDD,0xFF);
        SDL_RenderFillRect(REN, &r);
    }
    {
        // PIXEL
        SDL_Rect r = { G.x, G.y, DIM, DIM };
        SDL_SetRenderDrawColor(REN, 0xFF,0x00,0x00,0xFF);
        SDL_RenderFillRect(REN, &r);
    }

    if (trv) {
        SDL_SetRenderDrawColor(REN, 0xFF,0xFF,0xFF,0x77);
        SDL_RenderFillRect(REN, NULL);
    }

    SDL_RenderPresent(REN);
}

int cb_rec (tml_evt* evt) {
    SDL_Event sdl;
    assert(SDL_PollEvent(&sdl) != 0);

    switch (sdl.type) {
        case SDL_QUIT:
            *evt = (tml_evt) { TML_EVT_QUIT };
            return TML_RET_REC;
        case SDL_KEYDOWN: {
            int key = sdl.key.keysym.sym;
            if (key==SDLK_SPACE) {
                *evt = (tml_evt) { TML_EVT_JUMP };
                return TML_RET_REC;
            }
            if (key==SDLK_LEFT) {
                return TML_RET_TRV;
            }
            break;
        }
    }
    return TML_RET_NONE;
}

int cb_trv (int max, int cur, int* ret) {
    if (G.dead) {
        cur = MIN(cur,G.dead);
    }

    SDL_Event sdl;
    SDL_PollEvent(&sdl);

    switch (sdl.type) {
        case SDL_QUIT:
            exit(0);
            break;
        case SDL_KEYUP: {
            int key = sdl.key.keysym.sym;
            if (key == SDLK_LEFT) {
                return TML_RET_REC;
            }
            break;
        }
    }
    *ret = MAX(0, cur-1);
    return TML_RET_TRV;
}
