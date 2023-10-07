//
// Created by root on 10/3/23.
//

#include "../include/linux/sched.h"
#include "../include/linux/task.h"
#include "../include/linux/kernel.h"
#include "../include/asm/system.h"

extern void switch_task(task_t *task);

extern void switch_idle_task(task_t *task);

extern task_t *tasks[NR_TASKS];

task_t *find_ready_task() {
    task_t *next = NULL;

    // Start from index 1
    for (int i = 1; i < NR_TASKS; ++i) {
        task_t *task = tasks[i];

        if (NULL == task) continue;

        if (TASK_READY != task->state) continue;

        next = task;

        // If the intention is to return the first "ready" task found,
        // then it would be more efficient to break out of the for loop once a "ready" task is identified.
        // Without the break, the function continues to search through the remaining tasks and,
        // as currently written, would return the last "ready" task found.
        break;
    }

    return next;
}

task_t *current = NULL;

void sched() {
    task_t *next = find_ready_task();

    if (NULL == next) {
        current = tasks[0];

        switch_idle_task(tasks[0]);

        return;
    }

    next->state = TASK_RUNNING;

    current = next;

    switch_task(next);
}