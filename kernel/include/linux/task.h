//
// Created by root on 10/3/23.
//

#ifndef FUNSIZEOS_TASK_H
#define FUNSIZEOS_TASK_H

#include "types.h"
#include "mm.h"

#define NR_TASKS 64

typedef void *(*task_fun_t)(void *);

typedef struct task_t {
    int pid;
    int cr3;
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

task_t *create_task(char *name, task_fun_t fun);

void task_init();

#endif //FUNSIZEOS_TASK_H
