BUILDDIR := build
HELLO_SRC := kernel/hello/hello.asm
HELLO_OBJS := $(BUILDDIR)/hello/hello.o

.PHONY: hello clean bochs all

hello: $(HELLO_OBJS)

$(HELLO_OBJS): $(HELLO_SRC) | $(BUILDDIR)/hello
	nasm $< -o $@

$(BUILDDIR)/hello:
	mkdir -p $@

HD_IMG_NAME:=hd.img
SECTSIZE:=512
BOOT_SRC:=$(wildcard kernel/boot/*.asm)
BOOT_OBJ:=$(BOOT_SRC:kernel/boot/%.asm=${BUILDDIR}/boot/%.o)

all: $(HD_IMG_NAME)

$(HD_IMG_NAME): $(BOOT_OBJ)
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

${BUILDDIR}/boot/%.o: kernel/boot/%.asm
	# Create the directory of the target file if it doesn't already exist.
	@mkdir -p $(@D)
	@nasm $< -o $@

clean:
	@rm -rf $(BUILD)
	@rm -f $(HD_IMG_NAME)
	@rm -rf $(BUILDDIR) hello.img bx_enh_dbg.ini

bochs:
	bochs -q -f bochsrc
