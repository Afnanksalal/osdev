section .multiboot
align 4
    dd 0x1BADB002          ; magic
    dd 0x00                ; flags
    dd -0x1BADB002         ; checksum

section .text
global start
extern kernel_main

start:
    cli
    call kernel_main
hang:
    hlt
    jmp hang
