#include "keyboard.h"

#include "ps2.h"

namespace keyboard {
    static constexpr char SCAN_CODE_CHARACTERS[256] = {
        /* 00 */ '\0', /* 01 */ '\0', /* 02 */ '\0', /* 03 */ '\0',
        /* 04 */ '\0', /* 05 */ '\0', /* 06 */ '\0', /* 07 */ '\0',
        /* 08 */ '\0', /* 09 */ '\0', /* 0a */ '\0', /* 0b */ '\0',
        /* 0c */ '\0', /* 0d */ '\0', /* 0e */ '`',  /* 0f */ '\0',
        /* 10 */ '\0', /* 11 */ '\0', /* 12 */ '\0', /* 13 */ '\0',
        /* 14 */ '\0', /* 15 */ 'q',  /* 16 */ '1',  /* 17 */ '\0',
        /* 18 */ '\0', /* 19 */ '\0', /* 1a */ 'z',  /* 1b */ 's',
        /* 1c */ 'a',  /* 1d */ 'w',  /* 1e */ '2',  /* 1f */ '\0',
        /* 20 */ '\0', /* 21 */ 'c',  /* 22 */ 'x',  /* 23 */ 'd',
        /* 24 */ 'e',  /* 25 */ '4',  /* 26 */ '3',  /* 27 */ '\0',
        /* 28 */ '\0', /* 29 */ ' ',  /* 2a */ 'v',  /* 2b */ 'f',
        /* 2c */ 't',  /* 2d */ 'r',  /* 2e */ '5',  /* 2f */ '\0',
        /* 30 */ '\0', /* 31 */ 'n',  /* 32 */ 'b',  /* 33 */ 'h',
        /* 34 */ 'g',  /* 35 */ 'y',  /* 36 */ '6',  /* 37 */ '\0',
        /* 38 */ '\0', /* 39 */ '\0', /* 3a */ 'm',  /* 3b */ 'j',
        /* 3c */ 'u',  /* 3d */ '7',  /* 3e */ '8',  /* 3f */ '\0',
        /* 40 */ '\0', /* 41 */ ',',  /* 42 */ 'k',  /* 43 */ 'i',
        /* 44 */ 'o',  /* 45 */ '0',  /* 46 */ '9',  /* 47 */ '\0',
        /* 48 */ '\0', /* 49 */ '.',  /* 4a */ '/',  /* 4b */ 'l',
        /* 4c */ ';',  /* 4d */ 'p',  /* 4e */ '-',  /* 4f */ '\0',
        /* 50 */ '\0', /* 51 */ '\0', /* 52 */ '\'', /* 53 */ '\0',
        /* 54 */ '[',  /* 55 */ '=',  /* 56 */ '\0', /* 57 */ '\0',
        /* 58 */ '\0', /* 59 */ '\0', /* 5a */ '\n', /* 5b */ ']',
        /* 5c */ '\0', /* 5d */ '\\', /* 5e */ '\0', /* 5f */ '\0',
        /* 60 */ '\0', /* 61 */ '\0', /* 62 */ '\0', /* 63 */ '\0',
        /* 64 */ '\0', /* 65 */ '\0', /* 66 */ '\b', /* 67 */ '\0',
    };

    char poll_char() {
        for (;;) {
            uint8_t received = ps2::poll();
            if (received == 0xf0) {
                ps2::poll(); // Skip key releases.
                continue;
            }

            char ch = SCAN_CODE_CHARACTERS[received];
            if (ch) return ch;
        }
    }
}