#if 0
#!/bin/sh
gcc -Wall -I ../pico-sdl/src/ `sdl2-config --cflags` tml.c ../pico-sdl/src/pico.c test.c -o xtest `sdl2-config --libs` -lSDL2_image -lSDL2_ttf
exit
#endif

#include <assert.h>
#include "pico.h"
#include "tml.h"

enum {
    TML_DRAG = TML_NEXT
};

int main (void) {
    pico_open();
    tml_open(20);

    #define CARDS 2
    Pico_2i cs[CARDS] = {
        { -10, 0 },
        {  10, 0 }
    };

    pico_output((Pico_Output) {
        .tag = PICO_OUTPUT_SET,
        .Set = {
            .tag = PICO_OUTPUT_SET_COLOR,
            .Color = {
                .tag = PICO_OUTPUT_SET_COLOR_CLEAR,
                .Clear = {0xFF,0xFF,0xFF,0xFF}
            }
        }
    });

	while (1) {
        tml_evt evt = tml_wait();
        printf(">>> %d\n", evt.id);

        pico_output((Pico_Output) { .tag=PICO_OUTPUT_CLEAR });

        if (evt.id == TML_DRAG) {
            cs[evt.pay.i3._1] = (Pico_2i) { evt.pay.i3._2, evt.pay.i3._3 };
        }

        pico_output((Pico_Output) {
            .tag = PICO_OUTPUT_SET,
            .Set = {
                .tag = PICO_OUTPUT_SET_COLOR,
                .Color = {
                    .tag = PICO_OUTPUT_SET_COLOR_DRAW,
                    .Draw = {0xFF,0x00,0x00,0xFF}
                }
            }
        });

        for (int i=0; i<CARDS; i++) {
            pico_output((Pico_Output) {
                .tag = PICO_OUTPUT_DRAW,
                .Draw = {
                    .tag = PICO_OUTPUT_DRAW_RECT,
                    .Rect = { cs[i], {5,9} }
                }
            });
        }

        {
            SDL_Event inp;
            while (1) {
                int has = pico_input(&inp, (Pico_Input){
                    .tag = PICO_INPUT_EVENT,
                    .Event = {
                        .tag = PICO_INPUT_EVENT_POLL,
                        .type = SDL_ANY
                    }
                });
                if (!has) break;

                static int drag_is = 0;
                static int drag_i;
                static Pico_2i drag_src;

                switch (inp.type) {
                    case SDL_QUIT:
                        exit(0);
                    case SDL_MOUSEBUTTONUP:
                        if (drag_is) {
                            tml_emit((tml_evt) {
                                TML_DRAG,
                                { .i3 = {
                                    drag_i,
                                    cs[drag_i]._1 + (inp.button.x-drag_src._1),
                                    cs[drag_i]._2 + (inp.button.y-drag_src._2)
                                }}
                            });
                            drag_is = 0;
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
            }
        }
	}

	tml_close();
	return 0;
}

