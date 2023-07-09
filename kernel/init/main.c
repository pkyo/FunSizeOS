void kernel_main(void) {
    char *video = (char *) 0xb8000;

    *video = 'X';
}