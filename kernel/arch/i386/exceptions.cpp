#include <arch/i386/exceptions.h>

#include <stdarg.h>

#include <arch/i386/asm.h>
#include <arch/i386/gdt.h>
#include <arch/i386/idt.h>
#include <arch/i386/terminal.h>
#include <printf.h>

void kpanic(const char* format, ...) {
    va_list args;
    va_start(args, format);

    terminal::set_color(terminal::Color::WHITE, terminal::Color::RED);
    terminal::write_cstr("Kernel panic! ");
    vprintf(format, args);
    disable_interrupts();
    for (;;) hlt();
}

static constexpr char const* TABLE_NAME[] = {
    "GDT", "IDT", "LDT", "IDT"
};

static void kpanic_with_selector(const char* message, int selector) {
    bool external = (selector & 1) == 1;
    int index = (selector >> 3) & 0x1fff;
    const char* table = TABLE_NAME[(selector >> 1) & 0b11];

    kpanic("%s (Index 0x%x in %s%s)\n",
        message, index, table,
        external ? ". Exception originated externally to the processor" : "");
}

__attribute__((interrupt))
static void division_error(idt::InterruptFrame*) {
    kpanic("Division error");
}

__attribute__((interrupt))
static void bound_range_exceeded(idt::InterruptFrame*) {
    kpanic("Bound range exceeded");
}

__attribute__((interrupt))
static void invalid_opcode(idt::InterruptFrame*) {
    kpanic("Invalid opcode");
}

__attribute__((interrupt))
static void device_not_available(idt::InterruptFrame*) {
    kpanic("No FPU available");
}

__attribute__((interrupt))
static void double_fault(idt::InterruptFrame*) {
    kpanic("Error handling another exception");
}

__attribute__((interrupt))
static void invalid_tss(idt::InterruptFrame*, int selector) {
    kpanic_with_selector("Invalid Task State Segment", selector);
}

__attribute__((interrupt))
static void segment_not_present(idt::InterruptFrame*, int selector) {
    kpanic_with_selector("Segment not present", selector);
}

__attribute__((interrupt))
static void stack_segment_fault(idt::InterruptFrame*, int selector) {
    kpanic_with_selector("Stack segment fault", selector);
}

__attribute__((interrupt))
static void general_protection_fault(idt::InterruptFrame*, int selector) {
    kpanic_with_selector("General protection fault", selector);
}

__attribute__((interrupt))
static void page_fault(idt::InterruptFrame*, int error_code) {
    kpanic("Page fault (Error code: %x)", error_code);
}

__attribute__((interrupt))
static void x87_exception(idt::InterruptFrame*) {
    kpanic("x87 exception");
}

__attribute__((interrupt))
static void alignment_check(idt::InterruptFrame*, int) {
    kpanic("Alignment check failed");
}

__attribute__((interrupt))
static void simd_exception(idt::InterruptFrame*) {
    kpanic("SIMD exception");
}

__attribute__((interrupt))
static void virtualization_exception(idt::InterruptFrame*) {
    kpanic("Virtualization exception");
}

__attribute__((interrupt))
static void control_protection_exception(idt::InterruptFrame*, int) {
    kpanic("Control protection exception");
}

__attribute__((interrupt))
static void hypervisor_injection_exception(idt::InterruptFrame*) {
    kpanic("Hypervisor injection exception");
}

__attribute__((interrupt))
static void vmm_communication_exception(idt::InterruptFrame*, int) {
    kpanic("VMM communication exception");
}

__attribute__((interrupt))
static void security_exception(idt::InterruptFrame*, int) {
    kpanic("Security exception");
}

void register_exception_handlers() {
    idt::register_trap(0x00, division_error);
    idt::register_trap(0x05, bound_range_exceeded);
    idt::register_trap(0x06, invalid_opcode);
    idt::register_trap(0x07, device_not_available);
    idt::register_trap(0x08, double_fault);
    idt::register_trap(0x0a, invalid_tss);
    idt::register_trap(0x0b, segment_not_present);
    idt::register_trap(0x0c, stack_segment_fault);
    idt::register_trap(0x0d, general_protection_fault);
    idt::register_trap(0x0e, page_fault);
    idt::register_trap(0x10, x87_exception);
    idt::register_trap(0x11, alignment_check);
    idt::register_trap(0x13, simd_exception);
    idt::register_trap(0x14, virtualization_exception);
    idt::register_trap(0x15, control_protection_exception);
    idt::register_trap(0x1c, hypervisor_injection_exception);
    idt::register_trap(0x1d, vmm_communication_exception);
    idt::register_trap(0x1e, security_exception);
}
