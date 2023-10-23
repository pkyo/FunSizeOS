//
// Created by root on 10/3/23.
//

#include "../include/linux/kernel.h"
#include "../include/linux/task.h"
#include "../include/linux/sched.h"
#include "../include/linux/mm.h"
#include "../include/string.h"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
#pragma clang diagnostic ignored "-Wvoid-pointer-to-int-cast"

extern task_t *current;
extern int jiffy;
extern int cpu_tickes;

task_t *tasks[NR_TASKS] = {0};

int find_empty_process() {
    int ret = 0;

    bool is_find = false;

    for (int i = 0; i < NR_TASKS; ++i) {
        if (0 == tasks[i]) {
            is_find = true;

            ret = i;

            break;
        }
    }

    if (!is_find) {
        printk("no valid pid\n");

        return -1;
    }

    return ret;
}

task_t *create_task(char *name, task_fun_t fun, int priority) {
    // Memory Allocation for the Task
    task_union_t *task = (task_union_t *) kmalloc(4096);

    // Initialize Task Memory
    memset(task, 0, PAGE_SIZE);

    // Set Task Properties
    task->task.pid = find_empty_process();
    task->task.ppid = (NULL == current) ? 0 : current->pid;
    task->task.scheduling_times = 0;
    strcpy(task->task.name, name);
    tasks[task->task.pid] = &(task->task);

    // Set Task Execution Context
    // The cr3 register typically holds the base address of the page directory,
    // which is used for virtual memory and paging.
    task->task.tss.cr3 = (int) task + sizeof(task_t);
    task->task.tss.eip = (u32) fun; // Sets the instruction pointer (eip) to the entry function of the task.
    // Points to the top of the allocated memory, which will serve as the stack's top.
    task->task.esp0 = (int) task + PAGE_SIZE;
    task->task.ebp0 = task->task.esp0; // Initializes the base pointer to the top of the stack.
    task->task.tss.esp0 = task->task.esp0;
    task->task.state = TASK_READY;

    return &(task->task);
}

void *t1_fun(void *arg) {
    for (int i = 0; i < 0xffffffff; ++i) {
        printk("t1 task running...\n");
    }

    return NULL;
}

void *idle(void *arg) {
    create_task("t1", t1_fun, 1);

    while (true) {
        printk("idle task running...\n");

        __asm__ volatile ("sti;");
        __asm__ volatile ("hlt;");
    }
}

void task_init() {
    create_task("idle", idle, 1);
}

void task_exit(int code, task_t *task) {
    for (int i = 1; i < NR_TASKS; ++i) {
        task_t *tmp = tasks[i];

        if (task == tmp) {
            printk("task exit: %s\n", tmp->name);

            tmp->exit_code = code;

            tasks[i] = NULL;

            current = NULL;

            kfree_s(tmp, 4096);

            break;
        }
    }
}

void current_task_exit(int code) {
    for (int i = 1; i < NR_TASKS; ++i) {
        task_t *tmp = tasks[i];

        if (current == tmp) {
            printk("task exit: %s\n", tmp->name);

            tmp->exit_code = code;

            tasks[i] = NULL;

            current = NULL;

            break;
        }
    }
}

int inc_scheduling_times(task_t *task) {
    return task->scheduling_times++;
}

pid_t get_task_pid(task_t *task) {
    return task->pid;
}

pid_t get_task_ppid(task_t *task) {
    return task->ppid;
}

#pragma clang diagnostic pop