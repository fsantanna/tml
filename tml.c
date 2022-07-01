#include <SDL.h>
#include <assert.h>
#include "tml.h"

struct {
    uint32_t nxt;
    int      mpf;
    int      tick;
    int      n;
    tml_evt  queue[1];
} G = { -1, -1, -1, 0, {} };

void tml_open (int fps) {
    assert(1000%fps == 0);
    G.mpf = 1000 / fps;
    G.nxt = SDL_GetTicks();
}

void tml_close (void) {
}

tml_evt tml_wait (void) {
    if (G.n > 0) {
        assert(G.n == 1);
        return G.queue[--G.n];
    } else {
        uint32_t now = SDL_GetTicks();
        if (now < G.nxt) {
            SDL_WaitEventTimeout(NULL, G.nxt-now);
            now = SDL_GetTicks();
        }
        if (now >= G.nxt) {
            G.tick++;
            G.nxt += G.mpf;
            return (tml_evt) { TML_TICK, {.tick=G.tick} };
        } else {
            assert(SDL_HasEvents(SDL_FIRSTEVENT,SDL_LASTEVENT));
            return (tml_evt) { TML_NONE };
        }
    }
    assert(0);
}

void tml_emit (tml_evt evt) {
    assert(G.n == 0);
    G.queue[G.n++] = evt;
}

void tml_travel (int tick) {
    assert(0);
}
