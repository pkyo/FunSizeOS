//
// Created by root on 9/1/23.
//

#include "../../include/linux/kernel.h"
#include "../../include/linux/traps.h"
#include "../../include/asm/io.h"
#include "../../include/linux/sched.h"


#define PIT_CHAN0_REG 0X40                      // PIT Channel 0 data port
#define PIT_CHAN2_REG 0X42                      // PIT Channel 2 data port
#define PIT_CTRL_REG 0X43                       // PIT control port

#define HZ 100                                  // Defines the frequency of the timer as 100 Hz
#define OSCILLATOR 1193182                      // Defines the oscillator's frequency in Hz
#define CLOCK_COUNTER (OSCILLATOR / HZ)         // The divisor used to set the frequency of the PIT

#define JIFFY (1000 / HZ)                       // Number of milliseconds between each interrupt, which is 10 milliseconds in this case

int jiffy = JIFFY;
int cpu_ticks = 0;

// This code sets up the PIT to generate interrupts at a rate of 100 Hz, with each interrupt occurring every 10 milliseconds.
void clock_init() {
    // Writes to the PIT control port to configure Channel 0 in rate generator mode (0011)
    // and to allow reading/writing of the least significant byte followed by the most significant byte (0100).
    out_byte(PIT_CTRL_REG, 0b00110100);
    // Writes the least significant byte (LSB) of the CLOCK_COUNTER to PIT Channel 0 data port.
    out_byte(PIT_CHAN0_REG, CLOCK_COUNTER & 0xff);
    // Writes the most significant byte (MSB) of the CLOCK_COUNTER to PIT Channel 0 data port.
    out_byte(PIT_CHAN0_REG, (CLOCK_COUNTER >> 8) & 0xff);
}

void clock_handler(int idt_index) {
    send_eoi(idt_index);

    printk("set eoi 0x%x %d\n", idt_index, cpu_ticks);

    cpu_ticks++;

    do_timer();
}