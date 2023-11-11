.set FLAG_ALIGN,    1<<0 // Align loaded modules on page boundaries.
.set FLAG_MEMINFO,  1<<1 // Provide memory map.
.set FLAGS,         FLAG_ALIGN | FLAG_MEMINFO
.set MAGIC,         0x1badb002
.set CHECKSUM,      -(MAGIC + FLAGS)

.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top:

.section .text
.global _start
.type _start, @function
_start:
        cli
        mov     $stack_top, %esp

        call    kmain

        cli
1:      hlt
        jmp     1b

.size _start, . - _start
