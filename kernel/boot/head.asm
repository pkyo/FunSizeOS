[SECTION .text]
[BITS 32]
extern kernel_main
global _start

_start:
; The master PIC's command port is at I/O address 0x0020, and its data port is at 0x0021.
; The slave PIC's command port is at 0x00A0, and its data port is at 0x00A1.
.config_8259a:
    ; Sending ICW1 to the master PIC.
    ; ICW1 initializes the PIC and tells it to expect more initialization commands.

    ; You need to initialize the PICs by sending the initialization command (code 0x11).
    ; This command makes the PIC wait for 3 extra "initialization words" on the data port.
    ; These bytes give the PIC its vector offset (ICW2),
    ; tell it how it is wired to master/slaves (ICW3),
    ; and provide additional information about the environment (ICW4).
    mov al, 11h
    out 20h, al

    ; Sends ICW1 to the slave PIC
    out 0a0h, al

    ; Send ICW2 to the master PIC. ICW2 sets the base address of the interrupt vector.
    ; ICW2 is used to set the base address of the Interrupt Vector Table (IVT).
    mov al, 20h
    ; In the x86 architecture, the interrupt number for the keyboard is typically 1 or
    ; more specifically it's IRQ 1 (Interrupt Request 1).
    ; However, the actual interrupt vector number depends on the Interrupt Controller and its configuration.
    ; In systems with a PIC (Programmable Interrupt Controller), like the Intel 8259,
    ; the base interrupt vector number is often set to 32 (0x20 in hexadecimal) to
    ; avoid conflicts with the CPU's internal exceptions and interrupts,
    ; and the IRQ lines are mapped starting from this base.
    ; Thus, the keyboard interrupt (IRQ 1) is typically mapped to
    ; the interrupt vector number 33 (0x21 in hexadecimal).
    out 21h, al

    ; Send ICW2 to the slave PIC
    mov al, 28h
    out 0a1h, al

    ; Send ICW3 to the master PIC.
    ; ICW3 tells the master PIC there is a slave PIC at IRQ2 (binary 0000 0100).
    mov al, 04h
    out 21h, al

    ; Send ICW3 to the slave PIC.
    ; ICW3 tells the slave PIC its cascade identity (binary 0000 0010).
    mov al, 02h
    out 0a1h , al

    ; Send ICW4 to the master PIC. ICW4 sets the PIC to 8086 mode.
    mov al, 003h
    out 021h, al

    ; Sends ICW4 to the slave PIC.
    out 0a1h, al

; Disable all interrupts handled by the 8259A PIC except for the keyboard interrupt and clock interrupt.
.enable_8259a_main:
    ; mov al, 11111100b
    ; mov al, 11111111b
    mov al, 11111110b
    out 21h, al

; Mask or disable all interrupts from a slave PIC.
.disable_8259a_slave:
    mov al, 11111111b
    out 0a1h, al

.enter_c_word:
    call kernel_main
    jmp $