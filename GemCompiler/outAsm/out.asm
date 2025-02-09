section .text
global main
extern ExitProcess

main:
    mov rcx, 141
    call ExitProcess
