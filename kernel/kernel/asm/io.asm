[bits 32]
[SECTION .text]

; Declares in_byte as a global symbol,
; Makes it accessible from other assembly or C files.
global in_byte
in_byte:
    push ebp ; It's a common way to start a function in assembly, to preserve the calling environment.
    mov ebp, esp ; Sets up a new base pointer for this function

    xor eax, eax

    ; In C calling convention, this location holds the first argument passed to the function.
    ; In this case, it's the port number to read from
    mov edx, [ebp + 8]
    ; It reads a byte from the port specified by dx into the al register.
    in al, dx

    leave ; This instruction is equivalent to `mov esp, ebp` followed by `pop ebp`

    ; In C calling convention, the return value of a function is passed in eax,
    ; which in this case holds the byte read from the port.
    ret

global out_byte
out_byte:
    push ebp
    mov ebp, esp

    mov edx, [ebp + 8]
    mov eax, [ebp + 12]
    out dx, al

    leave

    ret

global in_word
in_word:
    push ebp
    mov ebp, esp

    xor eax, eax

    mov edx, [ebp + 8]
    in ax, dx

    leave

    ret

global out_word
out_word:
    push ebp
    mov ebp, esp

    mov edx, [ebp + 8]
    mov eax, [ebp + 12]
    out dx, ax

    leave

    ret