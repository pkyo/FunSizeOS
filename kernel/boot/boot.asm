[ORG 0x7c00]

[SECTION .data]
BOOT_MAIN_ADDR equ 0x500
THE_NUMBER_OF_THE_STARTING_SECTOR equ 2
THE_NUMBER_OF_SECTORS_TO_READ equ 2

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

    ; ecx is set to the number of the starting sector to read from the disk
    ; bl is set to the number of sectors to read
    mov ecx, THE_NUMBER_OF_THE_STARTING_SECTOR
    mov bl, THE_NUMBER_OF_SECTORS_TO_READ

    ; The next sequence of instructions sets up disk parameters by
    ; writing to the disk control ports from 0x1F2 to 0x1F7.
    mov dx, 0x1f2 ; 0x1F2: Number of sectors to read/write
    mov al, bl
    out dx, al

    inc dx ; 0x1F3: The low 8 bits of the LBA (Logical Block Addressing mode)
    mov al, cl
    out dx, al

    inc dx ; 0x1F4: bits 8-15 of the LBA (in LBA mode)
    mov al, ch
    out dx, al

    inc dx ; 0x1F5: bits 16-23 of the LBA (in LBA mode)
    shr ecx, 16
    mov al, cl
    out dx, al

    ; 0x1F6: This port has several functions, it selects the drive (master or slave),
    ; selects the head number (in CHS mode) or provides the top 4 bits of the LBA (in LBA mode).
    ; It also selects the addressing mode (CHS or LBA).
    inc dx
    mov al, ch
    ; Bits 0-3: These bits represent the highest four bits (bits 24-27) of the LBA address.
    ; Bit 4: This bit selects the hard drive. A 0 denotes the master drive, and a 1 denotes the slave drive.
    ; Bit 5: In the ATA specification, this bit is usually set to 1.
    ; Bit 6: This bit selects the addressing mode. A 0 means CHS mode is used, while a 1 means LBA mode is used.
    ; Bit 7: This bit is usually set to 1, as per the ATA specification.
    and al, 0b1110_1111
    out dx, al

    inc dx ; 0x1F7: Status command
    ; 0x20, the "read sectors" command
    ; This tells the hard drive to start reading data from the specified sector into its internal buffer
    mov al, 0x20
    out dx, al

.read_check:
    mov dx, 0x1f7
    ; This reads the value from the I/O port specified in dx (0x1f7, the hard drive's status register)
    in al, dx

    ; This isolates the third and seventh bits of the status register,
    ; which indicate the readiness of the drive (bit 3) and whether the drive is busy (bit 7).
    and al, 0b10001000
    cmp al, 0b00001000

    ; It will jump back to the .read_check label if the result of the previous cmp operation is not zero
    jnz .read_check

    mov dx, 0x1f0
    ; cx will be used as a counter in the loop for reading data.
    mov cx, 256
    mov edi, BOOT_MAIN_ADDR

.read_data:
    ; This reads the value from the I/O port specified in
    ; dx (0x1f0, the hard drive's status register) into the al register.
    in ax, dx
    mov [edi], ax
    add edi, 2
    ; This decrements the cx register by 1 and then jumps back to the .read_data label if cx is not zero.
    ; This forms a loop that will continue to read data from the hard drive into memory,
    ; 1 word at a time, until 256 words (512 bytes, or one sector) have been read.
    loop .read_data

    mov si, jump_to_setup_message
    call print

    jmp BOOT_MAIN_ADDR

jump_to_setup_message:
    ; In ASCII, the value 10 represents a newline (\n)
    ; In ASCII, the value 13 represents a carriage return (\r)
    db "FunSizeOS jump to setup ...", 10, 13, 0

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
; These specific values (0x55 and 0xaa) are known as the boot signature,
; which is a marker used by the BIOS to identify the MBR as a valid bootable device.
db 0x55, 0xaa