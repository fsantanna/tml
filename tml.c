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
    } S = { SDL_GetTicks()+mpf, mpf, 0 };

    struct {
        int tot;
        int nxt;
        tml_tick_evt buf[EVT_MAX];
    } Q = { 0, 0, {} };

    cb_sim((tml_evt) { TML_EVT_FIRST });

_RET_EVT_: {

    while (1) {
        if (Q.nxt < Q.tot) {
            cb_sim(Q.buf[Q.nxt++].evt);
        } else {
            uint32_t now = SDL_GetTicks();
            if (now < S.nxt) {
                SDL_WaitEventTimeout(NULL, S.nxt-now);
                now = SDL_GetTicks();
            }
            if (now >= S.nxt) {
                S.tick++;
                S.nxt += S.mpf;
                cb_sim((tml_evt) { TML_EVT_TICK, {.tick=S.tick} });
            }

            tml_evt evt;
            switch (cb_evt(&evt)) {
                case TML_RET_EVT:
                    assert(Q.tot < EVT_MAX);
                    Q.buf[Q.tot++] = (tml_tick_evt) { S.tick, evt };
                    break;
                case TML_RET_TRV:
                    goto _RET_TRV_;
            }
        }
    }
}

_RET_TRV_: {

    uint32_t prv = SDL_GetTicks();
    uint32_t nxt = SDL_GetTicks();
    while (1) {
        uint32_t now = SDL_GetTicks();
        if (now < nxt) {
            SDL_WaitEventTimeout(NULL, nxt-now);
            now = SDL_GetTicks();
        }
        if (now >= nxt) {
            nxt += S.mpf;
        }

        tml_trv trv;
        switch (cb_trv(&trv)) {
            case TML_RET_EVT:
                S.nxt += (SDL_GetTicks() - prv);
                goto _RET_EVT_;
                break;
            case TML_RET_TRV:
                goto _RET_TRV_;
        }
    }
}

#if 0
                    cb_sim((tml_evt) { TML_EVT_FIRST });
                    int tick = 0;
                    for (int i=0; i<Q.tot; i++) {
                        tml_tick_evt cur = Q.buf[i];
                        while (cur.tick > tick) {
                            tick++;
                            SDL_Delay(S.mpf);
                            cb_sim((tml_evt) { TML_EVT_TICK, {.tick=S.tick} });
                        }
                        cb_sim(Q.buf[i].evt);
                    }
                    break;
                }
#endif
}
