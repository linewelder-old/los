#pragma once

#include "idt.h"

namespace keyboard {
    typedef void (*KeypressCallback)(char key);

    void set_callback(KeypressCallback func);

    void irq_handler(idt::InterruptFrame*);
}
