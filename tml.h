enum {
    TML_RET_NONE = 0,
    TML_RET_EVT,
    TML_RET_TRV
};

enum {
    TML_EVT_FIRST = 0,
    TML_EVT_TICK,
    TML_EVT_NEXT
};

enum {
    TML_TRV_BAK = 0,
    TML_TRV_FWD
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

typedef struct {
    uint8_t id;
} tml_trv;

void tml_loop (int fps, void(*cb_sim)(tml_evt), int(*cb_evt)(tml_evt*), int(*cb_trv)(tml_trv*));
