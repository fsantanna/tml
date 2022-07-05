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
    TML_EVT_KEY = TML_EVT_NEXT
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
            G.pos = (SDL_Point) { 20, 20 };
            G.vel = (SDL_Point) { 0, 0 };
            break;
        case TML_EVT_TICK:
            G.pos.x += G.vel.x;
            G.pos.y += G.vel.y;
            break;
        case TML_EVT_KEY:
            switch (evt.pay.i1) {
                case SDLK_LEFT:  { G.vel.x=-1; G.vel.y=0; break; }
                case SDLK_RIGHT: { G.vel.x= 1; G.vel.y=0; break; }
                case SDLK_UP:    { G.vel.y=-1; G.vel.x=0; break; }
                case SDLK_DOWN:  { G.vel.y= 1; G.vel.x=0; break; }
                case SDLK_SPACE: { G.vel.x= 0; G.vel.y=0; break; }
            }
            break;
    }
}

void cb_eff (void) {
    SDL_SetRenderDrawColor(REN, 0xFF,0xFF,0xFF,0xFF);
    SDL_RenderClear(REN);
    SDL_SetRenderDrawColor(REN, 0xFF,0x00,0x00,0xFF);
    SDL_Rect r = { G.pos.x, G.pos.y, 10, 10 };
    SDL_RenderFillRect(REN, &r);
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
            if (key==SDLK_LEFT || key==SDLK_RIGHT ||
                key==SDLK_UP   || key==SDLK_DOWN  ||
                key==SDLK_SPACE
            ) {
                *evt = (tml_evt) { TML_EVT_KEY, {.i1=key} };
                return TML_RET_REC;
            }
            if (key==SDLK_ESCAPE) {
                return TML_RET_TRV;
            }
            break;
        }
    }
    return TML_RET_NONE;
}

int cb_trv (int max, int cur, int* ret) {
#if 0
    SDL_Event inp;
    int has = pico_input_event_poll(&inp, SDL_ANY);
    //assert(has);

    pico_output_set_color_draw_rgb(0x00,0xFF,0x00);
    pico_output_draw_pixel_xy(20,-20);

    Pico_4i r1 = {  0,-50,10,10};
    Pico_4i r2 = {-15,-50,10,10};
    Pico_4i r3 = { 15,-50,10,10};
    Pico_4i r4 = {-30,-50,10,10};
    Pico_4i r5 = { 30,-50,10,10};
    Pico_4i r6 = {-45,-50,10,10};
    Pico_4i r7 = { 45,-50,10,10};

    pico_output_draw_rect_4i(r1);
    pico_output_draw_rect_4i(r2);
    pico_output_draw_rect_4i(r3);
    pico_output_draw_rect_4i(r4);
    pico_output_draw_rect_4i(r5);
    pico_output_draw_rect_4i(r6);
    pico_output_draw_rect_4i(r7);
    //pico_output_set_image_crop_xywh(20,40,40,40);
    //pico_output_set_image_size_wh(20,20);
    //pico_output_draw_image(((Pico_2i){0,-20}), "media.jpg");
    pico_output_present();

    static int _going = 0;
    static int going = 0;

    switch (inp.type) {
        case SDL_QUIT:
            exit(0);
            break;
        case SDL_KEYDOWN: {
            int key = inp.key.keysym.sym;
            if (key==SDLK_ESCAPE) {
                going = _going = 0;
                return TML_RET_REC;
            }
            break;
        }
        case SDL_MOUSEBUTTONDOWN: {
            Pico_2i pt = { inp.button.x, inp.button.y };
            if (pico_isPointVsRect(pt, r1)) {
                if (going == 0) {
                    going = _going;
                } else {
                    _going = going;
                    going = 0;
                }
            } else if (pico_isPointVsRect(pt, r2)) {
                going = 0;
                if (cur > 0) {
                    *ret = cur - 1;
                    return TML_RET_TRV;
                }
            } else if (pico_isPointVsRect(pt, r3)) {
                going = 0;
                if (cur < max) {
                    *ret = cur + 1;
                    return TML_RET_TRV;
                }
            } else if (pico_isPointVsRect(pt, r4)) {
                going = MIN(0,going) - 1;
            } else if (pico_isPointVsRect(pt, r5)) {
                going = MAX(0,going) + 1;
            } else if (pico_isPointVsRect(pt, r6)) {
                going = 0;
                if (cur != 0) {
                    *ret = 0;
                    return TML_RET_TRV;
                }
            } else if (pico_isPointVsRect(pt, r7)) {
                going = 0;
                if (cur != max) {
                    *ret = max;
                    return TML_RET_TRV;
                }
                return TML_RET_TRV;
            }
        }
    }
    if (!has && going!=0) {
        *ret = MIN(max, MAX(0, cur+going));
        if (*ret != cur) {
            return TML_RET_TRV;
        }
    }
#endif
    return TML_RET_NONE;
}
