[ORG 0x7c00]

[SECTION .text]
[BITS 16]
global _start

_start:
    ; These two lines of code would change the video mode
    ; of the computer to 80x25 text mode(80 columns by 25 rows)
    mov ax, 3
    int 0x10

    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov si, ax

    mov si, msg
    call print

    ; $ This symbol refers to the current address being executed
    jmp $

msg:
    ; In ASCII, the value 10 represents a newline (\n)
    ; In ASCII, the value 13 represents a carriage return (\r)
    db "Hello FunSizeOS ...", 10, 13, 0

print:
    ; ah = 0x0E corresponds to the "Teletype output" service,
    ; which prints a single character on the console
    mov ah, 0x0e
    ; In the "Teletype output" service,
    ; bh selects the page number (usually 0) and
    ; bl selects the character attribute (color).
    ; Here, bl = 0x01 sets the color to blue.
    mov bh, 0
    mov bl, 0x01
.loop:
    mov al, [si]
    cmp al, 0
    jz .done
    ; This instruction triggers BIOS interrupt 0x10,
    ; which calls the service specified by ah (0x0E, Teletype output),
    ; and prints the character in al with the attributes specified by bh and bl
    int 0x10
    inc si
    jmp .loop
.done:
    ret

times 510 - ($ - $$) db 0
; These specific values (0x55 and 0xaa) are known as the hello signature,
; which is a marker used by the BIOS to identify the MBR as a valid bootable device.
db 0x55, 0xaa