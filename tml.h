enum {
    TML_NONE = 0,
    TML_FIRST,
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

void    tml_open   (int fps);
void    tml_close  (void);
tml_evt tml_wait   (void);
void    tml_emit   (tml_evt evt);
void    tml_travel (int tick);
