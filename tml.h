enum {
    TML_FIRST = 0,
    TML_TICK,
    TML_NEXT
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

void tml_loop   (int fps, void(*cb1)(tml_evt), int(*cb2)(tml_evt*));
void tml_travel (int tick);
