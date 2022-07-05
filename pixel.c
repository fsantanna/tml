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
void cb_eff (void);
int  cb_rec (tml_evt* evt);
int  cb_trv (int max, int cur, int* ret);

struct {
    SDL_Point pos;
    SDL_Point vel;
} G;

SDL_Renderer* REN = NULL;

int main (void) {
    assert(SDL_Init(SDL_INIT_VIDEO) == 0);

    SDL_Window* win = SDL_CreateWindow (
        "TML: Time Machine Library",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        400, 400,
        SDL_WINDOW_SHOWN
    );
    assert(win != NULL);

    REN = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    assert(REN != NULL);

    tml_loop(50, sizeof(G), &G, cb_sim, cb_eff, cb_rec, cb_trv);

    SDL_DestroyRenderer(REN);
    SDL_DestroyWindow(win);
    SDL_Quit();
}

void cb_sim (tml_evt evt) {
    switch (evt.id) {
        case TML_EVT_INIT:
            G.pos = (SDL_Point) { 0, 250 };
            G.vel = (SDL_Point) { 3, 5 };
            break;
        case TML_EVT_TICK:
            G.pos.x = (G.pos.x + G.vel.x) % 400;
            G.pos.y = MIN(250, G.pos.y+G.vel.y);
            G.vel.y = MIN(5, G.vel.y+1);
            break;
        case TML_EVT_JUMP:
            if (G.pos.y == 250) {
                G.vel.y = -10;
            }
            break;
    }
}

void cb_eff (void) {
    SDL_SetRenderDrawColor(REN, 0xFF,0xFF,0xFF,0xFF);
    SDL_RenderClear(REN);
    {
        SDL_Rect r = { 0, 260, 400, 40 };
        SDL_SetRenderDrawColor(REN, 0x4B,0x37,0x1C,0xFF);
        SDL_RenderFillRect(REN, &r);
    }
    {
        SDL_Rect r = { 200-20, 260, 40, 15 };
        SDL_SetRenderDrawColor(REN, 0x00,0x00,0xDD,0xFF);
        SDL_RenderFillRect(REN, &r);
    }
    {
        SDL_Rect r = { G.pos.x, G.pos.y, 10, 10 };
        SDL_SetRenderDrawColor(REN, 0xFF,0x00,0x00,0xFF);
        SDL_RenderFillRect(REN, &r);
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
