section .text
global main
extern ExitProcess   ; Declare the Win32 API function

main:
    mov rcx, 0       ; Exit code 0 (passed via RCX)

call ExitProcess ; Call Windows API