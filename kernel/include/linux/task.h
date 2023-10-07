//
// Created by root on 10/3/23.
//

#ifndef FUNSIZEOS_TASK_H
#define FUNSIZEOS_TASK_H

#include "types.h"
#include "mm.h"

#define NR_TASKS 64

typedef void *(*task_fun_t)(void *);

typedef enum task_state_t {
    TASK_INIT,
    TASK_RUNNING,
    TASK_READY,
    TASK_BLOCKED,
    TASK_SLEEPING,
    TASK_WAITING,
    TASK_DIED,
} task_state_t;

// The TSS is used by the processor to save and restore the context of a task during task switches.
typedef struct tss_t {
    u32 backlink;           // A link to the previous task, containing the segment selector of the previous task
    u32 esp0;               // The top of the stack address for ring0.
    u32 ss0;                // The stack segment selector for ring0.
    u32 esp1;               // The top of the stack address for ring1.
    u32 ss1;                // The stack segment selector for ring1.
    u32 esp2;               // The top of the stack address for ring2.
    u32 ss2;                // The stack segment selector for ring2.
    u32 cr3;
    u32 eip;
    u32 flags;
    u32 eax;
    u32 ecx;
    u32 edx;
    u32 ebx;
    u32 esp;
    u32 ebp;
    u32 esi;
    u32 edi;
    u32 es;
    u32 cs;
    u32 ss;
    u32 ds;
    u32 fs;
    u32 gs;
    u32 ldtr;               // Local descriptor table register selector.
    u16 trace: 1;           // A single bit that, when set, causes a debug exception during a task switch.
    u16 reversed: 15;       // 15 bits reserved for future use or not used.
    u16 iobase;
    u32 ssp;                // A pointer to the shadow stack for the task.
} __attribute__((packed)) tss_t;

typedef struct task_t {
    tss_t tss;
    int pid;
    int ppid;
    char name[32];
    task_state_t state;
    int exit_code;
    int counter;
    int priority;
    int scheduling_times;
    int esp0;
    int ebp0;
    int esp3;
    int ebp3;
    int magic;
} task_t;

// In a union, only one of the members can be used at any given time,
// so the size of the union is determined by its largest member.
// In this case, stack is clearly much larger than task.
// Therefore, the size of task_union_t will be the size of the stack,
// which is 4096 bytes or 4KB.
typedef union task_union_t {
    task_t task;
    char stack[PAGE_SIZE];
} task_union_t;

task_t *create_task(char *name, task_fun_t fun, int priority);

void task_init();

void task_exit(int code, task_t *task);

void current_task_exit(int code);

void task_sleep(int ms);

void task_wakeup();

int inc_scheduling_times(task_t *task);

pid_t get_task_pid(task_t *task);

pid_t get_task_ppid(task_t *task);

task_t *create_child(char *name, task_fun_t fun, int priority);

int get_esp3(task_t *task);

void set_esp3(task_t *task, int esp);

#endif //FUNSIZEOS_TASK_H
