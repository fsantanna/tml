#include <SDL.h>
#include <assert.h>
#include "tml.h"

#define EVT_MAX 10000

typedef struct {
    int     tick;
    tml_evt evt;
} tml_tick_evt;

void tml_loop (int fps, void(*cb_sim)(tml_evt), int(*cb_evt)(tml_evt*), int(*cb_trv)(tml_trv*)) {
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

    cb_sim((tml_evt) { TML_EVT_FIRST });

    while (1) {
        if (E.nxt < E.tot) {
            cb_sim(E.queue[E.nxt++].evt);
        } else {
            uint32_t now = SDL_GetTicks();
            if (now < T.nxt) {
                SDL_WaitEventTimeout(NULL, T.nxt-now);
                now = SDL_GetTicks();
            }
            if (now >= T.nxt) {
                T.tick++;
                T.nxt += T.mpf;
                cb_sim((tml_evt) { TML_EVT_TICK, {.tick=T.tick} });
            }

            tml_evt evt;
            switch (cb_evt(&evt)) {
                case TML_RET_EVT:
                    assert(E.tot < EVT_MAX);
                    E.queue[E.tot++] = (tml_tick_evt) { T.tick, evt };
                    break;
                case TML_RET_TRAVEL: {
                    uint32_t now = SDL_GetTicks();
                    cb_sim((tml_evt) { TML_EVT_FIRST });
                    int tick = 0;
                    for (int i=0; i<E.tot; i++) {
                        tml_tick_evt cur = E.queue[i];
                        while (cur.tick > tick) {
                            tick++;
                            SDL_Delay(T.mpf);
                            cb_sim((tml_evt) { TML_EVT_TICK, {.tick=T.tick} });
                        }
                        cb_sim(E.queue[i].evt);
                    }
                    T.nxt += (SDL_GetTicks() - now);
                    break;
                }
            }
        }
    }
}
