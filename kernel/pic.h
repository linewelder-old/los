#pragma once

#include <stdint.h>

namespace pic {
    void init(uint8_t pic1_vector_offset, uint8_t pic2_vector_offset);

    void set_mask(uint8_t irq);

    void clear_mask(uint8_t irq);

    void send_eoi(uint8_t irq);
}
