#if 0
#!/bin/sh
gcc -Wall -I ../pico-sdl/src/ `sdl2-config --cflags` tml.c ../pico-sdl/src/pico.c test.c -o xtest `sdl2-config --libs` -lSDL2_image -lSDL2_ttf
exit
#endif

#include <assert.h>
#include "pico.h"
#include "tml.h"

enum {
    TML_EVT_KEY = TML_EVT_NEXT,
    TML_EVT_DRAG
};

void cb_sim (tml_evt);
void cb_eff (void);
int  cb_rec (tml_evt* evt);
int  cb_trv (int max, int cur, int* ret);

int main (void) {
    pico_open();
    //pico_output_set_size_wh(640,480);
    pico_output_set_pixel_wh(4,4);
    //pico_output_set_grid(0);
    pico_output_set_color_clear_rgb(0xFF,0xFF,0xFF);
    pico_output_set_auto(0);

    tml_loop(20, cb_sim, cb_eff, cb_rec, cb_trv);
}

#define CARDS 2
Pico_2i cs[CARDS];
Pico_2i rp;
Pico_2i rv;

void cb_sim (tml_evt evt) {
    switch (evt.id) {
        case TML_EVT_FIRST:
            cs[0] = (Pico_2i) { -10, 0 };
            cs[1] = (Pico_2i) {  10, 0 };
            rp = (Pico_2i) { 0, 0 };
            rv = (Pico_2i) { 0, 0 };
            break;
        case TML_EVT_TICK:
            rp._1 += rv._1;
            rp._2 += rv._2;
            break;
        case TML_EVT_KEY:
            switch (evt.pay.i1) {
                case SDLK_LEFT:  { rv._1=-1; rv._2=0; break; }
                case SDLK_RIGHT: { rv._1= 1; rv._2=0; break; }
                case SDLK_UP:    { rv._2= 1; rv._1=0; break; }
                case SDLK_DOWN:  { rv._2=-1; rv._1=0; break; }
                case SDLK_SPACE: { rv._1= 0; rv._2=0; break; }
            }
            break;
        case TML_EVT_DRAG:
            cs[evt.pay.i3._1] = (Pico_2i) { evt.pay.i3._2, evt.pay.i3._3 };
            break;
    }
}

void cb_eff (void) {
    pico_output_clear();
    pico_output_set_color_draw_rgb(0xFF,0x00,0x00);
    for (int i=0; i<CARDS; i++) {
        pico_output_draw_rect(cs[i], ((Pico_2i){5,9}));
    }
    pico_output_draw_pixel(rp);
    pico_output_present();
}

int cb_rec (tml_evt* evt) {
    pico_output_set_color_draw_rgb(0xFF,0x00,0x00);
    pico_output_draw_pixel_xy(20,-20);
    pico_output_present();

    if (evt == NULL) {
        return TML_RET_NONE;
    }

    SDL_Event inp;
    //int has =
    pico_input_event_poll(&inp, SDL_ANY);
    //assert(has);

    static int drag_is = 0;
    static int drag_i;
    static Pico_2i drag_src;

    switch (inp.type) {
        case SDL_QUIT:
            exit(0);
            break;
        case SDL_KEYDOWN: {
            int key = inp.key.keysym.sym;
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
                        cs[drag_i]._1 + (inp.button.x-drag_src._1),
                        cs[drag_i]._2 + (inp.button.y-drag_src._2)
                    }}
                };
                drag_is = 0;
                return TML_RET_REC;
            }
            break;
        case SDL_MOUSEBUTTONDOWN: {
            Pico_2i pt = {inp.button.x, inp.button.y};
            for (int i=0; i<CARDS; i++) {
                Pico_4i r = { cs[i]._1, cs[i]._2, 5, 9 };
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

int cb_trv (int max, int cur, int* ret) {
    SDL_Event inp;
    //int has =
    pico_input_event_poll(&inp, SDL_ANY);
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

    static int going = 0;

    switch (inp.type) {
        case SDL_QUIT:
            exit(0);
            break;
        case SDL_KEYDOWN: {
            int key = inp.key.keysym.sym;
            if (key==SDLK_ESCAPE) {
                return TML_RET_REC;
            }
            break;
        }
        case SDL_MOUSEBUTTONDOWN: {
            Pico_2i pt = { inp.button.x, inp.button.y };
            if (pico_isPointVsRect(pt, r2)) {
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
                going = -1;
            } else if (pico_isPointVsRect(pt, r5)) {
                going = 1;
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
    if (going != 0) {
        if (going==-1 && cur>0) {
            *ret = cur - 1;
            return TML_RET_TRV;
        } else if (going==1 && cur<max) {
            *ret = cur + 1;
            return TML_RET_TRV;
        }
    }
    return TML_RET_NONE;
}
