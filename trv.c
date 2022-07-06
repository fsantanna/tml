#include <SDL2/SDL_image.h>

SDL_Texture* imgs[7];

void trv_init (void) {
	imgs[0] = IMG_LoadTexture(REN, "imgs/r0.png");
	imgs[1] = IMG_LoadTexture(REN, "imgs/r1.png");
	imgs[2] = IMG_LoadTexture(REN, "imgs/r2.png");
	imgs[3] = IMG_LoadTexture(REN, "imgs/r3.png");
	imgs[4] = IMG_LoadTexture(REN, "imgs/r4.png");
	imgs[5] = IMG_LoadTexture(REN, "imgs/r5.png");
	imgs[6] = IMG_LoadTexture(REN, "imgs/r6.png");
}

void trv_quit (void) {
    for (int i=0; i<7; i++) {
        SDL_DestroyTexture(imgs[i]);
    }
}

void trv_eff (void) {
    SDL_SetRenderDrawColor(REN, 0xFF,0xFF,0xFF,0x77);
    SDL_RenderFillRect(REN, NULL);
#if 0
    SDL_SetRenderDrawColor(REN, 0x00,0xFF,0x00,0xFF);
    SDL_RenderFillRect(REN, &r1);
#else
    for (int i=0; i<7; i++) {
        SDL_RenderCopy(REN, imgs[i], NULL, &trv_rs[i]);
    }
#endif
}

int trv_cb (SDL_Event* sdl, int max, int cur, int* ret) {
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
        switch (sdl->type) {
            case SDL_QUIT:
                return TML_RET_QUIT;
            case SDL_KEYDOWN: {
                int key = sdl->key.keysym.sym;
                if (key==SDLK_ESCAPE) {
                    going = _going = 0;
                    return TML_RET_REC;
                }
                break;
            }
            case SDL_MOUSEBUTTONDOWN: {
                SDL_Point pt = { sdl->button.x, sdl->button.y };
                for (int i=0; i<7; i++) {
                    if (SDL_PointInRect(&pt, &trv_rs[i])) {
                        switch (i) {
                            case 0:
                                if (going == 0) {
                                    going = _going;
                                } else {
                                    _going = going;
                                    going = 0;
                                }
                                break;
                            case 1:
                                going = 0;
                                if (cur > 0) {
                                    *ret = cur - 1;
                                    return TML_RET_TRV;
                                }
                                break;
                            case 2:
                                going = 0;
                                if (cur < max) {
                                    *ret = cur + 1;
                                    return TML_RET_TRV;
                                }
                                break;
                            case 3:
                                going = MIN(0,going) - 1;
                                break;
                            case 4:
                                going = MAX(0,going) + 1;
                                break;
                            case 5:
                                going = 0;
                                if (cur != 0) {
                                    *ret = 0;
                                    return TML_RET_TRV;
                                }
                                break;
                            case 6:
                                going = 0;
                                if (cur != max) {
                                    *ret = max;
                                    return TML_RET_TRV;
                                }
                                break;
                        }
                    }
                }
            }
        }
    }

    return TML_RET_NONE;
}
