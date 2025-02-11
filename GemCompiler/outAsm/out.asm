section .text
global main
extern ExitProcess

main:
    sub rsp, 40
    mov rcx, 141
    call ExitProcess
