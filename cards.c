#if 0
#!/bin/sh
gcc -Wall `sdl2-config --cflags` tml.c cards.c -o xcards `sdl2-config --libs` -lSDL2_image
exit
#endif

#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <SDL.h>
#include "tml.h"

enum {
    TML_EVT_DRAG = TML_EVT_NEXT
};

void cb_sim (tml_evt);
void cb_eff (int trv);
int  cb_rec (SDL_Event* sdl, tml_evt* evt);

#define FPS   50
#define WIN   400

#define CARDS 5
#define CARDW 30
#define CARDH 50

typedef struct {
    int n;
    int x;
    int y;
} Card;

struct {
    unsigned int seed;
    Card cards[CARDS];
} G;

unsigned int SEED;
SDL_Renderer* REN = NULL;

SDL_Rect r1 = {190,350,20,20};
//SDL_Rect r2 = {160,350,20,20};
//SDL_Rect r3 = {220,350,20,20};
SDL_Rect r2 = {154,350,12,20};
SDL_Rect r3 = {234,350,12,20};
SDL_Rect r4 = {110,350,20,20};
SDL_Rect r5 = {270,350,20,20};
SDL_Rect r6 = { 70,350,20,20};
SDL_Rect r7 = {310,350,20,20};

#include "trv.c"

int main (void) {
    SEED = time(NULL);
    assert(SDL_Init(SDL_INIT_VIDEO) == 0);

    SDL_Window* win = SDL_CreateWindow (
        "TML: Time Machine Library",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        WIN, WIN,
        SDL_WINDOW_SHOWN
    );
    assert(win != NULL);

    REN = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    assert(REN != NULL);
    SDL_SetRenderDrawBlendMode(REN,SDL_BLENDMODE_BLEND);

    trv_init();
    tml_loop(50, sizeof(G), &G, cb_sim, cb_eff, cb_rec, trv_cb);

    trv_quit();
    SDL_DestroyRenderer(REN);
    SDL_DestroyWindow(win);
    SDL_Quit();
}

void cb_sim (tml_evt evt) {
    switch (evt.id) {
        case TML_EVT_INIT:
            G.seed = SEED;
            for (int i=0; i<CARDS; i++) {
                G.cards[i] = (Card) { i+1, WIN/2-CARDW/2, WIN/2-CARDH/2 };
            }
            break;
        case TML_EVT_DRAG:
            G.cards[evt.pay.i3._1].x = evt.pay.i3._2;
            G.cards[evt.pay.i3._1].y = evt.pay.i3._3;
            break;
    }
}

void cb_eff (int trv) {
    SDL_SetRenderDrawColor(REN, 0xFF,0xFF,0xFF,0xFF);
    SDL_RenderClear(REN);

    SDL_SetRenderDrawColor(REN, 0xFF,0x00,0x00,0xFF);
    for (int i=0; i<CARDS; i++) {
        SDL_Rect r = { G.cards[i].x, G.cards[i].y, CARDW, CARDH };
        SDL_RenderFillRect(REN, &r);
    }

    if (trv) {
        trv_eff();
    }

    SDL_RenderPresent(REN);
}

int cb_rec (SDL_Event* sdl, tml_evt* evt) {
    static int drag_is = 0;
    static int drag_i;
    static SDL_Point drag_src;

    switch (sdl->type) {
        case SDL_QUIT:
            return TML_RET_QUIT;
        case SDL_KEYDOWN: {
            int key = sdl->key.keysym.sym;
            if (key == SDLK_ESCAPE) {
                return TML_RET_TRV;
            }
            break;
        }
        case SDL_MOUSEBUTTONDOWN: {
            SDL_Point pt = {sdl->button.x, sdl->button.y};
            for (int i=0; i<CARDS; i++) {
                SDL_Rect r = { G.cards[i].x, G.cards[i].y, CARDW, CARDH };
                if (SDL_PointInRect(&pt, &r)) {
                    drag_is  = 1;
                    drag_i   = i;
                    drag_src = pt;
                    break;
                }
            }
            break;
        }
        case SDL_MOUSEBUTTONUP:
            if (drag_is) {
                *evt = (tml_evt) {
                    TML_EVT_DRAG,
                    { .i3 = {
                        drag_i,
                        G.cards[drag_i].x + (sdl->button.x-drag_src.x),
                        G.cards[drag_i].y + (sdl->button.y-drag_src.y)
                    }}
                };
                drag_is = 0;
                return TML_RET_REC;
            }
            break;
    }
    return TML_RET_NONE;
}
