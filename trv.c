void cb_trv_eff (void) {
    SDL_SetRenderDrawColor(REN, 0xFF,0xFF,0xFF,0x77);
    SDL_RenderFillRect(REN, NULL);

    SDL_SetRenderDrawColor(REN, 0x00,0xFF,0x00,0xFF);
    SDL_RenderFillRect(REN, &r1);
    SDL_RenderFillRect(REN, &r2);
    SDL_RenderFillRect(REN, &r3);
    SDL_RenderFillRect(REN, &r4);
    SDL_RenderFillRect(REN, &r5);
    SDL_RenderFillRect(REN, &r6);
    SDL_RenderFillRect(REN, &r7);
    //pico_output_set_image_crop_xywh(20,40,40,40);
    //pico_output_set_image_size_wh(20,20);
    //pico_output_draw_image(((Pico_2i){0,-20}), "media.jpg");
}

int cb_trv (SDL_Event* sdl, int max, int cur, int* ret) {
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
