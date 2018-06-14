#include <stdio.h>

#define MSG_START 0x5E
#define MSG_END 0x24
#define SYMBOL 0x7E

int msg_code(void *dest, const void *src, int size)
{
    unsigned char const *from = src;
    unsigned char *to = dest;
    int i, n;

    for (i = 0, n = 0; i < size; i++) {
        if(from[i] == MSG_START) {
            to[n++] = SYMBOL;
            to[n++] = 0x01;
        } else if (from[i] == MSG_END) {
            to[n++] = SYMBOL;
            to[n++] = 0x02;
        } else if (from[i] == SYMBOL) {
            to[n++] = SYMBOL;
            to[n++] = 0x00;
        } else
            to[n++] = from[i];
    }
    return n;
}

int msg_decode(char *dest, const char *src, int size)
{
    unsigned char const *from = src;
    unsigned char *to = dest;
    int i, n;

    for (i = 0, n = 0; i < size; i++, n++) {
        if (from[i] == SYMBOL) {
            if (from[i + 1] == 0x00)
                to[n] = SYMBOL;
            else if (from[i + 1] == 0x01)
                to[n] = MSG_START;
            else if (from[i + 1] == 0x02)
                to[n] = MSG_END;
            i++;

        } else
            to[n] = from[i];
    }
    return n;
}

int main(void)
{
    unsigned char src[] = {0x00, 0x5E, 0x5E, 0x30, 0x45, 0x24, 0x7D, 0x7E};
    unsigned char dest[256];
    int i, n;

    for (i = 0; i < sizeof(src); i++) {
        printf("0x%X, ", src[i]);
    }
    printf("\n");

    n = msg_code(dest, src, sizeof(src));
    for (i = 0; i < n; i++) {
        printf("0x%X, ", dest[i]);
    }
    printf("\n");

    n = msg_decode(dest, dest, n);
    for (i = 0; i < n; i++) {
        printf("0x%X, ", dest[i]);
    }
    printf("\n");
    return 0;
}
