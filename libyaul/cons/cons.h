#ifndef _CONS_H_
#define _CONS_H_

/* Half the normal size */
#define COLS            40
#define ROWS            28
#define TAB_WIDTH       2

struct cons {
        struct {
                uint16_t col;
                uint16_t row;
        } cursor;

        void (*write)(struct cons *, int, uint8_t, uint8_t);
        void *driver;
};

void cons_write(struct cons *, const char *);

#endif /* !_CONS_H_ */
