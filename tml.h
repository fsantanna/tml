enum {
    TML_RET_NONE = 0,
    TML_RET_REC,
    TML_RET_TRV
};

enum {
    TML_EVT_FIRST = 0,
    TML_EVT_TICK,
    TML_EVT_NEXT
};

typedef struct {
    uint8_t id;
    union {
        int tick;
        int i1;
        struct {
            int _1;
            int _2;
        } i2;
        struct {
            int _1;
            int _2;
            int _3;
        } i3;
    } pay;
} tml_evt;

void tml_loop (int fps, void(*cb_sim)(tml_evt), void(*cb_eff)(void), int(*cb_rec)(tml_evt*), int(*cb_trv)(int,int,int*));
