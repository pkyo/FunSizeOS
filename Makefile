SECTSIZE:=512
BUILDDIR:=./build
BOOT:=kernel/boot
INIT:=kernel/init
KERNEL:=kernel/kernel
# -m32: This option tells the compiler to generate code that will run on a 32-bit machine or in a 32-bit environment.
# -masm=intel: This option tells the GCC compiler to output assembly code in Intel syntax, instead of the default AT&T syntax.
# -fno-builtin: This option tells the compiler not to recognize built-in functions which do not begin with an underscore in the source code.
# -nostdinc: This option tells the compiler not to search the standard system directories for header files.
# -fno-pic and -fno-pie: These options disable the generation of position independent code (PIC) and position independent executables (PIE), respectively.
# -nostdlib: This option tells the linker not to use the standard system libraries and startup files when linking. It is used to avoid linking against libraries which may not be compatible with the bare metal environment where the OS code is supposed to run.
# -fno-stack-protector: This option disables the automatic insertion of code that checks for buffer overflows, such as the checks that are performed by the functions __builtin_trap and __builtin_object_size.
CFLAGS:=$(strip -m32 -masm=intel -fno-builtin -nostdinc -fno-pic -fno-pie -nostdlib -fno-stack-protector)
DEBUG:= -g
HD_IMG_NAME:= "hd.img"
BOOT_SRC:=$(wildcard kernel/boot/boot.asm kernel/boot/setup.asm)
BOOT_OBJ:=$(BOOT_SRC:kernel/boot/%.asm=${BUILDDIR}/boot/%.o)

.PHONY: hello clean bochs all

all: ${BUILDDIR}/$(HD_IMG_NAME)

${BUILDDIR}/$(HD_IMG_NAME): $(BOOT_OBJ) ${BUILDDIR}/system.bin
	@rm -rf $(HD_IMG_NAME)
	# bximage is a utility that comes with the Bochs emulator.
	# It's used to create new disk images for the emulator
	# 	-q: Quick mode. The program asks fewer questions.
	# 	-hd=16: Creates a hard disk image of 16MB.
	# 	-func=create: Specifies that a new disk image is being created.
	# 	-sectsize=$(SECTSIZE): Sets the sector size of the disk image to whatever is stored in $(SECTSIZE).
	# 	-imgmode=flat: Creates the disk image in "flat" mode. In this mode, the disk image is a single file that directly represents the contents of the disk.
	@bximage -q -hd=16 -func=create -sectsize=$(SECTSIZE) -imgmode=flat $(HD_IMG_NAME)
	# The if stands for "input file", and of stands for "output file"
	# 	bs=$(SECTSIZE): Set both input and output block size to whatever is stored in $(SECTSIZE).
	# 	seek=0 and seek=1: Specifies the block number in the output file where writing will start.
	# 		For boot.o, it starts writing at the beginning of the file.
	# 		For setup.o, it starts writing at the second block.
	# 	count=1 and count=2: Specifies how many blocks of input to take.
	# 		For boot.o, it takes one block, and
	# 		for setup.o, it takes two blocks.
	# 	conv=notrunc: This tells dd not to truncate the output file.
	@dd if=${BUILDDIR}/boot/boot.o of=$(HD_IMG_NAME) bs=$(SECTSIZE) seek=0 count=1 conv=notrunc
	@dd if=${BUILDDIR}/boot/setup.o of=$(HD_IMG_NAME) bs=$(SECTSIZE) seek=1 count=2 conv=notrunc
	@dd if=${BUILDDIR}/system.bin of=$(HD_IMG_NAME) bs=$(SECTSIZE) seek=3 count=60 conv=notrunc

${BUILDDIR}/system.bin: ${BUILDDIR}/kernel.bin
	# objcopy: It is a utility provided by the GNU Binutils package.
	# In this context, it is used to copy or convert object files.
	# Here, it is used with the -O binary option, which instructs objcopy to convert the input file ($<, which is ${BUILDDIR}/kernel.bin)
	# to binary format. The output is written to the target file ($@, which is ${BUILDDIR}/system.bin).
	@objcopy -O binary $< $@
	# It is another utility provided by the GNU Binutils package.
	# It is used to list the symbols from object files.
	# In this case, it is used to list the symbols from the input file ($<, which is ${BUILDDIR}/kernel.bin)
	@nm $< | sort > ${BUILDDIR}/system.map

${BUILDDIR}/kernel.bin: ${BUILDDIR}/boot/head.o ${BUILDDIR}/init/main.o
	# ld: It is the GNU linker, used to link object files and libraries into an executable or a final binary.
	# -m elf_i386: This flag specifies the target format as ELF (Executable and Linkable Format) for the 32-bit x86 architecture.
	# -Ttext 0x1200: This flag sets the starting address of the text (code) section to 0x1200 in the output file.
	@ld -m elf_i386 $^ -o $@ -Ttext 0x1200

${BUILDDIR}/init/main.o: ${INIT}/main.c
	@mkdir -p $(dir $@)
	@gcc ${CFLAGS} ${DEBUG} -c $< -o $@

${BUILDDIR}/boot/head.o: ${BOOT}/head.asm
	# -f elf32: Specifies the output format as ELF (Executable and Linkable Format) for 32-bit architecture.
	# -g: Generates debugging information for the object file
	@nasm -f elf32 -g $< -o $@

${BUILDDIR}/boot/%.o: ${BOOT}/%.asm
	# Create the directory of the target file if it doesn't already exist.
	@mkdir -p $(@D)
	@nasm $< -o $@

clean:
	@rm -f $(HD_IMG_NAME)
	@rm -rf $(BUILDDIR) hello.img bx_enh_dbg.ini

bochs:
	@bochs -q -f bochsrc
