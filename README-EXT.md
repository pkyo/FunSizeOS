# Real Mode - Memory Layout

![LayoutTable](./docs/img/2.png)

In ROM, the physical address **0xFFFF0** typically contains a jump instruction, specifically `jmp 0xf000:0xe05b`. This
instruction is crucial for the boot-up process of a computer.

The ROM chip contains several components, primarily focused on hardware diagnostics, detection, and initialization. This
is a critical phase during the startup of a computer, where the system checks and prepares its hardware components for
operation.

Additionally, the ROM includes a set of software routines that facilitate interaction with peripheral devices. These
routines allow for data input from or output to these peripherals without requiring the user to understand the intricate
details of the hardware.

The software routines provided in the BIOS (Basic Input/Output System) are fundamental and cover standard
functionalities. This makes the BIOS a pivotal component in managing basic communications between the operating system
and hardware devices.

The ROM-BIOS plays a key role in the boot process by reading the contents of the hard drive's master boot record (MBR)
and loading it into the memory at the address **0x0000:0x7c00**. Following this, the BIOS executes a `jmp` instruction
to this
memory address, continuing the boot sequence.