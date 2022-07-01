#include <SDL.h>
#include <assert.h>
#include "tml.h"

#define EVT_MAX 10000

typedef struct {
    int     tick;
    tml_evt evt;
} tml_tick_evt;

void tml_loop (int fps, void(*cb1)(tml_evt), int(*cb2)(tml_evt*)) {
    int mpf = 1000 / fps;
    assert(1000%fps == 0);
    struct {
        uint32_t nxt;
        int      mpf;
        int      tick;
    } T = { SDL_GetTicks()+mpf, mpf, 0 };

    struct {
        int tot;
        int nxt;
        tml_tick_evt queue[EVT_MAX];
    } E = { 0, 0, {} };

    cb1((tml_evt) { TML_FIRST });

    while (1) {
        if (E.nxt < E.tot) {
            cb1(E.queue[E.nxt++].evt);
        } else {
            uint32_t now = SDL_GetTicks();
            if (now < T.nxt) {
                SDL_WaitEventTimeout(NULL, T.nxt-now);
                now = SDL_GetTicks();
            }
            if (now >= T.nxt) {
                T.tick++;
                T.nxt += T.mpf;
                cb1((tml_evt) { TML_TICK, {.tick=T.tick} });
            } else {
                assert(SDL_HasEvents(SDL_FIRSTEVENT,SDL_LASTEVENT));
                tml_evt evt;
                if (cb2(&evt)) {
                    assert(E.tot < EVT_MAX);
                    E.queue[E.tot++] = (tml_tick_evt) { T.tick, evt };
                }
            }
        }
    }
}

void tml_travel (int tick) {
#if 0
    tml_emit((tml_evt){ TML_FIRST });
    int tick = 0;
    for (int i=0; i<E.tot; i++) {
        tml_tick_evt cur = E.queue[i];
        if (cur.tick > tick) {
            tick = cur.tick;
            tml_emit((tml_evt) { TML_TICK, {.tick=T.tick} });
        }
        tml_emit(E.queue[i].evt);
    }
    tml_emit(
    assert(0);
#endif
}
