[ORG 0x500]

%macro COMPUTE_G_DB_AVL_LIMIT 2
    db (%1 | ((%2 >> 16) & 0xf))
%endmacro

[SECTION .data]
KERNEL_ADDR equ 0x1200

ARDS_TIMES_BUFFER       equ 0x1100
ARDS_BUFFER             equ 0x1102
ARDS_TIMES              dw 0
CHECK_BUFFER_OFFSET     dw 0

[SECTION .gdt] ; Global Descriptor Table (GDT)
SEG_BASE equ 0
SEG_LIMIT equ 0xfffff

B8000_SEG_BASE equ 0xb8000
B8000_SEG_LIMIT equ 0x7fff

; CODE_SELECTOR and DATA_SELECTOR are effectively set to 8 and 16 respectively
; When CODE_SELECTOR is broken down, it can be seen to contain the following parts:
;   1. Requested Privilege Level (RPL): The lowest two bits (bit 0 and bit 1) define the Requested Privilege Level
;   2. Table Indicator (TI): The second bit (bit 2) is the Table Indicator.
;       If the TI is 0, the selector is describing a segment in the GDT.
;       If the TI is 1, it is describing a segment in the LDT
;   3. Index: The top 13 bits (bit 3 to bit 15) are the index,
;       which defines the position of the segment in the descriptor table
CODE_SELECTOR equ (1 << 3)
DATA_SELECTOR equ (2 << 3)
B8000_SELECTOR equ (3 << 3)

; This line initializes the base of the GDT with a zero address and limit
gdt_base:
    ; db (Define Byte) declare a byte-sized variable or to reserve one byte of memory space
    ; dw (Define Word) declare a word-sized variable or to reserve two bytes of memory space
    ; dd (Define Doubleword) declare a double-word sized variable or to reserve four bytes of memory space
    dd 0, 0

; Each segment descriptor is 8 bytes long, and defines the properties of a segment
; Here's how the 8-byte structure is laid out:
;   Limit (0-15): The lower 16 bits of the limit value, which specifies the size of the segment. The actual limit value is calculated based on this field and the G flag in the flag field.
;   Base (0-15): The lower 16 bits of the base address of the segment. The base address is a 32-bit value that is the starting address of the segment.
;   Base (16-23): The next 8 bits of the base address.
;   Type: Specifies the segment type, and determines how the segment can be accessed and what operations are allowed to be performed on the segment.
;   S (descriptor type): Specifies whether the descriptor is for a system segment (S=0) or a code/data segment (S=1).
;   DPL (descriptor privilege level): Specifies the privilege level of the segment, ranging from 0 (most privileged) to 3 (least privileged).
;   P (segment present): Specifies whether the segment is present in memory (P=1) or not (P=0).
;   Limit (16-19): The upper 4 bits of the limit value.
;   Available for system use (AVL): Can be used by system software.
;   L (64-bit code segment): Determines whether the segment is a 64-bit code segment (L=1) or not (L=0).
;   D/B (default operation size): Determines the default length for effective addresses and operands within the segment.
;   G (granularity): Determines whether the limit is specified in bytes (G=0) or in pages (G=1).
;   Base (24-31): The top 8 bits of the base address.
gdt_code:
    dw SEG_LIMIT & 0xffff
    dw SEG_BASE & 0xffff
    db SEG_BASE >> 16 & 0xff
    ; P_DPL_S_TYPE
    ; P is the segment-present flag (must be 1)
    ; DPL is the descriptor privilege level (00 for kernel mode)
    ; S is the descriptor type (1 for code or data segments)
    ; TYPE defines the exact type of the segment. 1000 indicates a code segment that can be executed and read
    db 0b1_00_1_1000
    ; G_DB_L_AVL_LIMIT
    ; G is the granularity flag (0 for byte granularity, 1 for 4KB granularity)
    ; DB is the default operation size (1 for 32-bit segment, 0 for 16-bit segment)
    ; L (64-bit code segment): Determines whether the segment is a 64-bit code segment (L=1) or not (L=0).
    ; AVL is available for use by system software
    ; LIMIT defines the high nibble of the segment limit.
    COMPUTE_G_DB_AVL_LIMIT 0b11000000, SEG_LIMIT ; 0b1_1_0_0_0000
    db SEG_BASE >> 24 & 0xf

gdt_data:
    dw SEG_LIMIT & 0xffff
    dw SEG_BASE & 0xffff
    db SEG_BASE >> 16 & 0xff
    ; P_DPL_S_TYPE
    ; P is the segment-present flag (must be 1)
    ; DPL is the descriptor privilege level (00 for kernel mode)
    ; S is the descriptor type (1 for code or data segments)
    ; TYPE defines the exact type of the segment. 0010 indicates a data segment that can be read and written
    db 0b1_00_1_0010
    ; G_DB_L_AVL_LIMIT
    ; G is the granularity flag (0 for byte granularity, 1 for 4KB granularity)
    ; DB is the default operation size (1 for 32-bit segment, 0 for 16-bit segment)
    ; L (64-bit code segment): Determines whether the segment is a 64-bit code segment (L=1) or not (L=0).
    ; AVL is available for use by system software
    ; LIMIT defines the high nibble of the segment limit.
    COMPUTE_G_DB_AVL_LIMIT 0b11000000, SEG_LIMIT ; 0b1_1_0_0_0000
    db SEG_BASE >> 24 & 0xf

gdt_b8000:
    dw B8000_SEG_LIMIT & 0xffff
    dw B8000_SEG_BASE & 0xffff
    db B8000_SEG_BASE >> 16 & 0xff
    ; P_DPL_S_TYPE
    db 0b1_00_1_0010
    ; G_DB_L_AVL_LIMIT
    COMPUTE_G_DB_AVL_LIMIT 0b01000000, SEG_LIMIT ; 0b0_1_0_0_0000
    db B8000_SEG_BASE >> 24 & 0xf

gdt_ptr:
    ; $ - gdt_base computes the size of the GDT by subtracting
    ; the starting address of the GDT (gdt_base) from the current address ($),
    ; which is the address right after the last defined descriptor.
    ; This is stored as a word (16 bits)
    dw $ - gdt_base
    ; Here it's defining the starting address of the GDT.
    ; So this line is storing the address of gdt_base as a doubleword (32 bits)
    dd gdt_base

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

[SECTION .text]
[BITS 16]
global setup_start

setup_start:
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov si, ax

    mov si, preparing_to_enter_protected_mode
    call print

; `INT 0x15` is a BIOS interrupt call on the x86 architecture, and it provides a variety of functions.
; When we refer to memory-related queries, especially when EAX is set to 0xE820,
; we're typically discussing querying the system's physical address map.
;   Input Parameters:
;       EAX: Function number. Setting it to 0xE820 indicates querying the system address map.
;       EDX: Should be set to the signature "SMAP" (0x534D4150) prior to the call.
;       EBX: Used for pagination. On initial call, it should be set to 0, and after each call, the BIOS will return a new value,
;           which should be used for the subsequent call until a returned value of 0.
;       ECX: Size of the returned structure. Typically set to either 20 or 24, depending on the level of detail you wish to obtain.
;       ES:DI: Points to a buffer where the BIOS will place the returned data.
;   Output Parameters:
;       EAX: Returns the signature "SMAP" (0x534D4150) as a confirmation.
;       ES:DI: The buffer will be populated with the returned data.
;       EBX: Used for pagination. If it returns 0, no more data blocks are available.
;       ECX: The size of the data that was actually populated into the buffer.
;       CF (Carry Flag): If set to 1, it indicates an error.
memory_check:
    xor ebx, ebx            ; Clear ebx to 0
    mov di, ARDS_BUFFER     ; Set ES:DI to point to a memory location

.loop:
    ; Conduct a memory inspection using the BIOS `int 0x15` call with the function `0xe820`
    ; (which is used to query system address map information).
    mov eax, 0xe820                 ; Set AX to 0xe820
    mov ecx, 20                     ; Set CX to 20
    mov edx, 0x534D4150             ; Set DX to the "SMAP" signature
    int 0x15                        ; BIOS interrupt call

    jc memory_check_error           ; Jump to error handling if there's an error

    add di, cx                      ; Point to the next memory structure

    inc dword [ARDS_TIMES]          ; Increment the memory inspection counter

    cmp ebx, 0                      ; During inspection, BIOS will modify EBX. If EBX is not zero, continue inspecting.
    jne .loop

    mov ax, [ARDS_TIMES]            ; Store the memory inspection count
    mov [ARDS_TIMES_BUFFER], ax

    mov [CHECK_BUFFER_OFFSET], di   ; Store the offset

.memory_check_success:
    mov si, memory_check_success_msg
    call print

enter_protected_mode:
    ; This instruction clears the interrupt flag, disabling all maskable interrupts.
    ; This is done because you don't want an interrupt to occur while you are switching to protected mode
    cli
    ; Loads the Global Descriptor Table (GDT) register with the GDT defined by gdt_ptr
    lgdt [gdt_ptr]
    ; Reads the value at port 92h (also known as the fast A20 gate) into the al register
    in al, 92h
    ; Enables the A20 line by setting the second bit of the value in al register
    or al, 00000010b
    ; Writes the value back to port 92h, thus enabling the A20 line
    out 92h, al
    ; Moves the value in control register cr0 to eax register.
    mov eax, cr0
    ; Sets the least significant bit of eax, which is the Protected Mode Enable (PE) bit in cr0 register
    or eax, 1
    ; Writes the value back to cr0, thus enabling protected mode.
    mov cr0, eax

    xchg bx, bx

    ; CODE_SELECTOR equ (1 << 3), the CODE_SELECTOR is defined to be 8,
    ; which in binary is 1000. This means the RPL is 0, the TI is 0, and the index is 1.
    ; Therefore, the CODE_SELECTOR is pointing to the second entry (index starts at 0) in the GDT
    ; with a requested access of privilege level 0.
    jmp CODE_SELECTOR:protected_mode

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

memory_check_error:
    mov si, memory_check_error_msg
    call print

    jmp $

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

preparing_to_enter_protected_mode:
    db "FunsizeOS is preparing to enter protected mode ... ", 10, 13, 0

memory_check_error_msg:
    db "FunsizeOS memory check fail ...", 10, 13, 0

memory_check_success_msg:
    db "FunsizeOS memory check success ...", 10, 13, 0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

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

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

[BITS 32]
protected_mode:
    mov ax, DATA_SELECTOR
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; This instruction sets the stack pointer esp to the physical address 0x9fbff.
    ; This address is likely the top of the available memory for the stack
    mov esp, 0x9fbff

    mov edi, KERNEL_ADDR
    ; This includes specifying the number of sectors to read (bl),
    ; and the starting sector (ecx are used for this purpose).
    mov ecx, 3
    mov bl, 60
    call read_hd

    jmp CODE_SELECTOR:KERNEL_ADDR

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

read_hd:
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
    shr ecx, 8
    ; Bits 0-3: These bits represent the highest four bits (bits 24-27) of the LBA address.
    ; Bit 4: This bit selects the hard drive. A 0 denotes the master drive, and a 1 denotes the slave drive.
    ; Bit 5: In the ATA specification, this bit is usually set to 1.
    ; Bit 6: This bit selects the addressing mode. A 0 means CHS mode is used, while a 1 means LBA mode is used.
    ; Bit 7: This bit is usually set to 1, as per the ATA specification.
    and cl, 0b1111
    mov al, 0b1110_0000
    or al, cl
    out dx, al

    inc dx ; 0x1F7: Status command
    ; 0x20, the "read sectors" command
    ; This tells the hard drive to start reading data from the specified sector into its internal buffer
    mov al, 0x20
    out dx, al

    mov cl, bl

.start_read:
    ; This is usually done to preserve the original value of cx since
    ; it will be changed in the following operations, and we want to restore it afterwards
    push cx

    call .wait_hd_ready
    call read_hd_data

    ; Restores the original value of cx from the stack
    pop cx
    ; The loop instruction is a loop construct in assembly.
    ; This will decrement the cx register and jump back to the .start_read label if cx is not zero.
    loop .start_read
.return:
    ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

.wait_hd_ready:
    ; This reads the value from the I/O port specified in dx (0x1f7, the hard drive's status register)
    mov dx, 0x1f7
.check:
    in al, dx
    ; This isolates the third and seventh bits of the status register,
    ; which indicate the readiness of the drive (bit 3) and whether the drive is busy (bit 7).
    and al, 0b1000_1000
    cmp al, 0b0000_1000
    ; It will jump back to the .check label if the result of the previous cmp operation is not zero
    jnz .check
    ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

read_hd_data:
    mov dx, 0x1f0
    ; cx will be used as a counter in the loop for reading data.
    mov cx, 256
.read_word:
    ; This reads the value from the I/O port specified in
    ; dx (0x1f7, the hard drive's status register) into the al register.
    in ax, dx
    mov [edi], ax
    add edi, 2
    ; This decrements the cx register by 1 and then jumps back to the .read_data label if cx is not zero.
    ; This forms a loop that will continue to read data from the hard drive into memory,
    ; 1 word at a time, until 256 words (512 bytes, or one sector) have been read.
    loop .read_word
    ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;