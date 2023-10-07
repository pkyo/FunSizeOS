[BITS 32]
[SECTION .text]

extern task_exit
extern sched
extern inc_scheduling_times
extern get_task_ppid
extern current

global switch_idle_task
switch_idle_task:
    ; Loads the address stored in the current variable into the eax register.
    ; This points to the current task's context.
    mov eax, [current]

    mov esp, [eax + 4]          ; esp0
    mov ebp, [eax + 15 * 4]     ; ebp

    mov ecx, [eax + 11 * 4]     ; ecx
    mov edx, [eax + 12 * 4]     ; edx
    mov ebx, [eax + 13 * 4]     ; ebx
    mov esi, [eax + 16 * 4]     ; esi
    mov edi, [eax + 17 * 4]     ; edi

    mov eax, [eax + 8 * 4]      ; eip

    sti

    jmp eax

    sti
    ; Halts the processor.
    ; This instruction is executed if, for some reason,
    ; the code doesn't successfully jump to the idle task.
    ; It's a protective measure to ensure the processor doesn't run unintended code.
    hlt

global switch_task
switch_task:
    mov eax, [current]

    mov esp, [eax + 4]              ; esp0
    mov ebp, [eax + 15 * 4]         ; ebp

    ; The `get_task_ppid` function takes its input argument from the eax register
    ; and returns its output (the ppid) in the eax register as well.
    push eax
    call get_task_ppid
    add esp, 4

    ; If the ppid is not 0, it indicates a child process.
    cmp eax, 0

    jne .recover_env

    mov eax, [current]
    push eax
    call inc_scheduling_times
    add esp, 4

    cmp eax, 0

    ; If the task is being scheduled for the first time
    jne .recover_env

    ; Scheduled for the first time
    mov eax, task_exit_handler
    push eax

.recover_env:
    mov eax, [current]

    mov ecx, [eax + 11 * 4]         ; ecx
    mov edx, [eax + 12 * 4]         ; edx
    mov ebx, [eax + 13 * 4]         ; ebx
    mov esi, [eax + 16 * 4]         ; esi
    mov edi, [eax + 17 * 4]         ; edi

    mov eax, [eax + 8 * 4]          ; eip

    sti

    jmp eax

task_exit_handler:
    mov eax, [current]

    push eax
    push 0
    call task_exit
    add esp, 8

    call sched

    sti
    hlt