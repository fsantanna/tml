#if 0
#!/bin/sh
gcc -Wall -I ../pico-sdl/src/ `sdl2-config --cflags` tml.c ../pico-sdl/src/pico.c test.c -o xtest `sdl2-config --libs` -lSDL2_image -lSDL2_ttf
exit
#endif

#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include "pico.h"
#include "tml.h"

enum {
    TML_EVT_KEY = TML_EVT_NEXT,
    TML_EVT_DRAG
};

void cb_sim (tml_evt evt);
void cb_eff (int trv);
int  cb_rec (SDL_Event* sdl, tml_evt* evt);
int  cb_trv (SDL_Event* sdl, int max, int cur, int* ret);

unsigned int SEED;

#define CARDS 2
struct {
    unsigned int seed;
    Pico_2i cs[CARDS];
    struct {
        Pico_2i pos;
        Pico_2i vel;
    } pxs[2];
} G;

Pico_4i r1 = {  0,-50,10,10};
Pico_4i r2 = {-15,-50,10,10};
Pico_4i r3 = { 15,-50,10,10};
Pico_4i r4 = {-30,-50,10,10};
Pico_4i r5 = { 30,-50,10,10};
Pico_4i r6 = {-45,-50,10,10};
Pico_4i r7 = { 45,-50,10,10};

int main (void) {
    pico_open();
    //pico_output_set_size_wh(640,480);
    pico_output_set_pixel_wh(5,5);
    //pico_output_set_grid(0);
    pico_output_set_color_clear_rgb(0xFF,0xFF,0xFF);
    pico_output_set_auto(0);

    SEED = time(NULL);
    tml_loop(20, sizeof(G), &G, cb_sim, cb_eff, cb_rec, cb_trv);
}

void cb_sim (tml_evt evt) {
    switch (evt.id) {
        case TML_EVT_INIT:
            G.seed = SEED;
            G.cs[0] = (Pico_2i) { -10, 0 };
            G.cs[1] = (Pico_2i) {  10, 0 };
            G.pxs[0].pos = (Pico_2i) { 0, 0 };
            G.pxs[0].vel = (Pico_2i) { 0, 0 };
            G.pxs[1].pos = (Pico_2i) { 0, 0 };
            G.pxs[1].vel = (Pico_2i) { 0, 0 };
            break;
        case TML_EVT_TICK:
            G.pxs[0].pos._1 += G.pxs[0].vel._1;
            G.pxs[0].pos._2 += G.pxs[0].vel._2;

            int r1 = rand_r(&G.seed) % 3 - 1;
            int r2 = rand_r(&G.seed) % 3 - 1;
            G.pxs[1].pos._1 += r1;
            G.pxs[1].pos._2 += r2;
            break;
        case TML_EVT_KEY:
            switch (evt.pay.i1) {
                case SDLK_LEFT:  { G.pxs[0].vel._1=-1; G.pxs[0].vel._2=0; break; }
                case SDLK_RIGHT: { G.pxs[0].vel._1= 1; G.pxs[0].vel._2=0; break; }
                case SDLK_UP:    { G.pxs[0].vel._2= 1; G.pxs[0].vel._1=0; break; }
                case SDLK_DOWN:  { G.pxs[0].vel._2=-1; G.pxs[0].vel._1=0; break; }
                case SDLK_SPACE: { G.pxs[0].vel._1= 0; G.pxs[0].vel._2=0; break; }
            }
            break;
        case TML_EVT_DRAG:
            G.cs[evt.pay.i3._1] = (Pico_2i) { evt.pay.i3._2, evt.pay.i3._3 };
            break;
    }
}

void cb_eff (int trv) {
    pico_output_clear();

    pico_output_set_color_draw_rgb(0xFF,0x00,0x00);
    for (int i=0; i<CARDS; i++) {
        pico_output_draw_rect(G.cs[i], ((Pico_2i){5,9}));
    }
    pico_output_draw_pixel(G.pxs[0].pos);
    pico_output_draw_pixel(G.pxs[1].pos);

    if (trv) {
        pico_output_set_color_draw_rgb(0x00,0xFF,0x00);
        pico_output_draw_pixel_xy(20,-20);

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
    } else {
        pico_output_set_color_draw_rgb(0xFF,0x00,0x00);
        pico_output_draw_pixel_xy(20,-20);
    }

    pico_output_present();
}

int cb_rec (SDL_Event* sdl, tml_evt* evt) {
    static int drag_is = 0;
    static int drag_i;
    static Pico_2i drag_src;

    pico_event_from_sdl(sdl, SDL_ANY);
    switch (sdl->type) {
        case SDL_QUIT:
            exit(0);
            break;
        case SDL_KEYDOWN: {
            int key = sdl->key.keysym.sym;
            if (key==SDLK_LEFT || key==SDLK_RIGHT ||
                key==SDLK_UP   || key==SDLK_DOWN  ||
                key==SDLK_SPACE
            ) {
                //SDL_Rect r = { 190, 190, 20, 20 };
                //SDL_SetRenderDrawColor(ren, 0x77,0x77,0x77,0x77);
                //SDL_RenderFillRect(ren, &r);
                *evt = (tml_evt) { TML_EVT_KEY, {.i1=key} };
                return TML_RET_REC;
            }
            if (key==SDLK_ESCAPE) {
                return TML_RET_TRV;
            }
            break;
        }
        case SDL_MOUSEBUTTONUP:
            if (drag_is) {
                *evt = (tml_evt) {
                    TML_EVT_DRAG,
                    { .i3 = {
                        drag_i,
                        G.cs[drag_i]._1 + (sdl->button.x-drag_src._1),
                        G.cs[drag_i]._2 + (sdl->button.y-drag_src._2)
                    }}
                };
                drag_is = 0;
                return TML_RET_REC;
            }
            break;
        case SDL_MOUSEBUTTONDOWN: {
            Pico_2i pt = {sdl->button.x, sdl->button.y};
            for (int i=0; i<CARDS; i++) {
                Pico_4i r = { G.cs[i]._1, G.cs[i]._2, 5, 9 };
                if (pico_isPointVsRect(pt,r)) {
                    drag_is  = 1;
                    drag_i   = i;
                    drag_src = pt;
                    break;
                }
            }
            break;
        }
    }
    return TML_RET_NONE;
}

int cb_trv (SDL_Event* sdl, int max, int cur, int* ret) {
    static int _going = 0;
    static int going = 0;

    if (sdl == NULL) {
        if (going != 0) {
            *ret = MIN(max, MAX(0, cur+going));
            if (*ret != cur) {
                return TML_RET_TRV;
            }
        }
    } else {
        pico_event_from_sdl(sdl, SDL_ANY);
        switch (sdl->type) {
            case SDL_QUIT:
                exit(0);
                break;
            case SDL_KEYDOWN: {
                int key = sdl->key.keysym.sym;
                if (key==SDLK_ESCAPE) {
                    going = _going = 0;
                    return TML_RET_REC;
                }
                break;
            }
            case SDL_MOUSEBUTTONDOWN: {
                Pico_2i pt = { sdl->button.x, sdl->button.y };
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
    }

    return TML_RET_NONE;
}
