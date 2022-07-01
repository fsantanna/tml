#include <SDL.h>
#include <assert.h>
#include "tml.h"

#define EVT_MAX 10000

typedef struct {
    int     tick;
    tml_evt evt;
} tml_tick_evt;

struct {
    struct {
        uint32_t nxt;
        int      mpf;
        int      tick;
    } t;
    struct {
        int max;
        int nxt;
        tml_tick_evt queue[EVT_MAX];
    } e;
} G = { {-1, -1, -1}, {0, 0, {}} };

void tml_open (int fps) {
    assert(1000%fps == 0);
    G.t.mpf = 1000 / fps;
    G.t.nxt = SDL_GetTicks();
}

void tml_close (void) {
}

tml_evt tml_wait (void) {
    if (G.t.tick == -1) {
        G.t.tick = 0;
        return (tml_evt) { TML_FIRST };
    } else if (G.e.nxt < G.e.max) {
        return G.e.queue[G.e.nxt++].evt;
    } else {
        uint32_t now = SDL_GetTicks();
        if (now < G.t.nxt) {
            SDL_WaitEventTimeout(NULL, G.t.nxt-now);
            now = SDL_GetTicks();
        }
        if (now >= G.t.nxt) {
            G.t.tick++;
            G.t.nxt += G.t.mpf;
            return (tml_evt) { TML_TICK, {.tick=G.t.tick} };
        } else {
            assert(SDL_HasEvents(SDL_FIRSTEVENT,SDL_LASTEVENT));
            return (tml_evt) { TML_NONE };
        }
    }
    assert(0);
}

void tml_emit (tml_evt evt) {
    assert(G.e.max < EVT_MAX);
    G.e.queue[G.e.max++] = (tml_tick_evt) { G.t.tick, evt };
}

void tml_travel (int tick) {
    assert(0);
}
