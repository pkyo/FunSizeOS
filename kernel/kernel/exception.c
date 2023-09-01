//
// Created by root on 9/1/23.
//

#include "../include/linux/kernel.h"
#include "../include/linux/types.h"

// This array contains strings that correspond to various CPU exceptions like "Divide Error", "Page Fault", etc.
char *messages[] = {
        "#DE Divide Error\0",
        "#DB RESERVED\0",
        "--  NMI Interrupt\0",
        "#BP Breakpoint\0",
        "#OF Overflow\0",
        "#BR BOUND Range Exceeded\0",
        "#UD Invalid Opcode (Undefined Opcode)\0",
        "#NM Device Not Available (No Math Coprocessor)\0",
        "#DF Double Fault\0",
        "    Coprocessor Segment Overrun (reserved)\0",
        "#TS Invalid TSS\0",
        "#NP Segment Not Present\0",
        "#SS Stack-Segment Fault\0",
        "#GP General Protection\0",
        "#PF Page Fault\0",
        "--  (Intel reserved. Do not use.)\0",
        "#MF x87 FPU Floating-Point Error (Math Fault)\0",
        "#AC Alignment Check\0",
        "#MC Machine Check\0",
        "#XF SIMD Floating-Point Exception\0",
        "#VE Virtualization Exception\0",
        "#CP Control Protection Exception\0",
};

// It takes an index to find the corresponding message and then prints it.
void div_zero_exception(int idt_index) {
    printk("%s\n", messages[idt_index]);
}

// A function that handles "General Protection" exceptions.
// It prints the corresponding message from the messages array.
void normal_protect_exception(int idt_index) {
    printk("%s\n", messages[idt_index]);
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

// A more general function to handle various types of CPU exceptions.
// This function also prints out additional diagnostic information like CPU flags,
// instruction pointer, stack pointer, etc.
void exception_handler(int idt_index,
                       int edi, int esi, int ebp, int esp,
                       int ebx, int edx, int ecx, int eax,
                       int eip, char cs, int eflags) {
    // During an interrupt or exception: EIP CS EFLAGS
    // These are typically automatically pushed onto the stack by the CPU when an interrupt or an exception occurs.
    // These registers are usually popped off the stack automatically when executing the `iret` (Interrupt Return) instruction.
    // The typical order for these pushes is as follows:
    //      EFLAGS
    //      CS
    //      EIP
    // The pushad instruction will sequentially push the following registers onto the stack:
    //      EAX
    //      ECX
    //      EDX
    //      EBX
    //      ESP
    //      EBP
    //      ESI
    //      EDI
    printk("EXCEPTION : %s \n", messages[idt_index]);
    printk("   VECTOR : 0x%02X\n", idt_index);
    printk("   EFLAGS : 0x%08X\n", eflags);
    printk("       CS : 0x%02X\n", cs);
    printk("      EIP : 0x%08X\n", eip);
    printk("      ESP : 0x%08X\n", esp);

    while (true);
}

#pragma clang diagnostic pop