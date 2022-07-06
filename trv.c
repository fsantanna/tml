#include <SDL2/SDL_image.h>

SDL_Texture *img1, *img2, *img3, *img4, *img5, *img6, *img7;

void trv_init (void) {
	img1 = IMG_LoadTexture(REN, "imgs/r1.png");
	img2 = IMG_LoadTexture(REN, "imgs/r2.png");
	img3 = IMG_LoadTexture(REN, "imgs/r3.png");
	img4 = IMG_LoadTexture(REN, "imgs/r4.png");
	img5 = IMG_LoadTexture(REN, "imgs/r5.png");
	img6 = IMG_LoadTexture(REN, "imgs/r6.png");
	img7 = IMG_LoadTexture(REN, "imgs/r7.png");
}

void trv_quit (void) {
	SDL_DestroyTexture(img1);
	SDL_DestroyTexture(img2);
	SDL_DestroyTexture(img3);
	SDL_DestroyTexture(img4);
	SDL_DestroyTexture(img5);
	SDL_DestroyTexture(img6);
	SDL_DestroyTexture(img7);
}

void trv_eff (void) {
    SDL_SetRenderDrawColor(REN, 0xFF,0xFF,0xFF,0x77);
    SDL_RenderFillRect(REN, NULL);

#if 0
    SDL_SetRenderDrawColor(REN, 0x00,0xFF,0x00,0xFF);
    SDL_RenderFillRect(REN, &r1);
    SDL_RenderFillRect(REN, &r2);
    SDL_RenderFillRect(REN, &r3);
    SDL_RenderFillRect(REN, &r4);
    SDL_RenderFillRect(REN, &r5);
    SDL_RenderFillRect(REN, &r6);
    SDL_RenderFillRect(REN, &r7);
#else
    SDL_RenderCopy(REN, img1, NULL, &r1);
    SDL_RenderCopy(REN, img2, NULL, &r2);
    SDL_RenderCopy(REN, img3, NULL, &r3);
    SDL_RenderCopy(REN, img4, NULL, &r4);
    SDL_RenderCopy(REN, img5, NULL, &r5);
    SDL_RenderCopy(REN, img6, NULL, &r6);
    SDL_RenderCopy(REN, img7, NULL, &r7);
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
                if (SDL_PointInRect(&pt, &r1)) {
                    if (going == 0) {
                        going = _going;
                    } else {
                        _going = going;
                        going = 0;
                    }
                } else if (SDL_PointInRect(&pt, &r2)) {
                    going = 0;
                    if (cur > 0) {
                        *ret = cur - 1;
                        return TML_RET_TRV;
                    }
                } else if (SDL_PointInRect(&pt, &r3)) {
                    going = 0;
                    if (cur < max) {
                        *ret = cur + 1;
                        return TML_RET_TRV;
                    }
                } else if (SDL_PointInRect(&pt, &r4)) {
                    going = MIN(0,going) - 1;
                } else if (SDL_PointInRect(&pt, &r5)) {
                    going = MAX(0,going) + 1;
                } else if (SDL_PointInRect(&pt, &r6)) {
                    going = 0;
                    if (cur != 0) {
                        *ret = 0;
                        return TML_RET_TRV;
                    }
                } else if (SDL_PointInRect(&pt, &r7)) {
                    going = 0;
                    if (cur != max) {
                        *ret = max;
                        return TML_RET_TRV;
                    }
                    return TML_RET_TRV;
                }
            }
        }
    }

    return TML_RET_NONE;
}
