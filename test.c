#if 0
#!/bin/sh
gcc -Wall -I ../pico-sdl/src/ `sdl2-config --cflags` tml.c ../pico-sdl/src/pico.c test.c -o xtest `sdl2-config --libs` -lSDL2_image -lSDL2_ttf
exit
#endif

#include <assert.h>
#include "pico.h"
#include "tml.h"

enum {
    TML_KEY = TML_NEXT,
    TML_DRAG
};

void cb1 (tml_evt);
int  cb2 (tml_evt* evt);

int main (void) {
    pico_open();
    pico_output_set_auto(0);
    pico_output_set_color_clear(0xFF,0xFF,0xFF);

    tml_loop(20, cb1, cb2);
}

#define CARDS 2
Pico_2i cs[CARDS] = {
    { -10, 0 },
    {  10, 0 }
};

Pico_2i rp = { 0, 0 };
Pico_2i rv = { 0, 0 };

void cb1 (tml_evt evt) {
    printf(">>> %d / %d\n", evt.id, evt.pay.tick);
    pico_output_clear();

    switch (evt.id) {
        case TML_TICK:
            rp._1 += rv._1;
            rp._2 += rv._2;
            break;
        case TML_KEY:
            switch (evt.pay.i1) {
                case SDLK_LEFT:  { rv._1=-1; rv._2=0; break; }
                case SDLK_RIGHT: { rv._1= 1; rv._2=0; break; }
                case SDLK_UP:    { rv._2= 1; rv._1=0; break; }
                case SDLK_DOWN:  { rv._2=-1; rv._1=0; break; }
                case SDLK_SPACE: { rv._1= 0; rv._2=0; break; }
            }
            break;
        case TML_DRAG:
            cs[evt.pay.i3._1] = (Pico_2i) { evt.pay.i3._2, evt.pay.i3._3 };
            break;
    }

    pico_output_set_color_draw(0xFF,0x00,0x00);

    for (int i=0; i<CARDS; i++) {
        pico_output_draw_rect(cs[i], ((Pico_2i){5,9}));
    }

    pico_output_draw_pixel(rp);
    pico_output_present();
}

int cb2 (tml_evt* evt) {
    SDL_Event inp;
    //int has =
    pico_input_event_poll(SDL_ANY);
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
                *evt = (tml_evt) { TML_KEY, {.i1=key} };
                return 1;
            }
            break;
        }
        case SDL_MOUSEBUTTONUP:
            if (drag_is) {
                *evt = (tml_evt) {
                    TML_DRAG,
                    { .i3 = {
                        drag_i,
                        cs[drag_i]._1 + (inp.button.x-drag_src._1),
                        cs[drag_i]._2 + (inp.button.y-drag_src._2)
                    }}
                };
                drag_is = 0;
                return 1;
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
    return 0;
}
