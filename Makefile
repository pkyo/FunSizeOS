BUILDDIR := build
HELLO_SRC := kernel/hello/hello.asm
HELLO_OBJS := $(BUILDDIR)/hello/hello.o

.PHONY: hello clean bochs

hello: $(HELLO_OBJS)

$(HELLO_OBJS): $(HELLO_SRC) | $(BUILDDIR)/hello
	nasm $< -o $@

$(BUILDDIR)/hello:
	mkdir -p $@

clean:
	rm -rf $(BUILDDIR) hello.img bx_enh_dbg.ini

bochs:
	bochs -q -f bochsrc
