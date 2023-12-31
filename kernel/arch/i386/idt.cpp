#include <arch/i386/idt.h>

namespace idt {
    static EncodedIdtEntry idt[256];

    static void register_gate(int vector, void* handler, uint8_t attribute_type) {
        idt[vector].offset_1 = static_cast<uint16_t>((uint32_t)handler & 0xffff);
        idt[vector].offset_2 = static_cast<uint16_t>((uint32_t)handler >> 16);
        idt[vector].segment = 0x08;
        idt[vector].zero = 0;
        idt[vector].attributes = ATTR_VALID | ATTR_RING(3) | attribute_type;
    }

    void register_interrupt(int vector, Handler handler) {
        register_gate(vector, reinterpret_cast<void*>(handler), ATTR_32_BIT_INTERRUPT);
    }

    void register_trap(int vector, Handler handler) {
        register_gate(vector, reinterpret_cast<void*>(handler), ATTR_32_BIT_TRAP);
    }

    void register_trap(int vector, ErrorCodeHandler handler) {
        register_gate(vector, reinterpret_cast<void*>(handler), ATTR_32_BIT_TRAP);
    }

    struct __attribute__((packed)) {
        uint16_t size;
        uint32_t base;
    } idtr;

    __attribute__((interrupt))
    static void void_handler(idt::InterruptFrame*) {}

    void init() {
        for (int i = 0; i < 256; i++) {
            register_interrupt(i, void_handler);
        }

        idtr.size = sizeof(idt);
        idtr.base = (uint32_t)&idt;
        asm volatile("lidt %0" : : "m"(idtr));
    }
}
