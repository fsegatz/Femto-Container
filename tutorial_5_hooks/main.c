/*
 * Copyright (c) 2021 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       BPF tutorial 1 example
 *
 * @author      Fabian Segatz <segatz@kth.se>
 *
 * @}
 */
#define N 1

#include <stdio.h>

#include "bpf.h"
#include "blob/fc/increment.bin.h"

#include "thread.h"
#include "xtimer.h"
#include "timex.h"

char container_thread_stack[THREAD_STACKSIZE_MAIN];

/* Pre-allocated stack for the virtual machine */
static uint8_t _stack[512] = { 0 };

void *container_thread(void *arg) {
    (void) arg;
    /* Context value to pass to the VM */
    uint64_t ctx = 0;
    int64_t result = 0;

    while (1) {
        puts("Executing container hook...");
        bpf_hook_execute(BPF_HOOK_SCHED, &ctx, sizeof(ctx), &result);
        puts("Hook executed.");
        xtimer_sleep(N);
    }

    return NULL;
}

int main(void)
{
    /* Initialize the bpf subsystem */
    bpf_init();

    puts("All up, running the Femto-Container application now");

    /* Define the application */
    bpf_t bpf = {
        .application = increment_bin,             /* The increment.bin content */
        .application_len = sizeof(increment_bin), /* Length of the application */
        .stack = _stack,                          /* Preallocated stack */
        .stack_size = sizeof(_stack),             /* And the length */
    };

    bpf_hook_t hook = {
        .application = &bpf,
    };



    bpf_setup(&bpf);

    bpf_hook_install(&hook, BPF_HOOK_SCHED);

    thread_create(container_thread_stack, sizeof(container_thread_stack),
                  THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
                  container_thread, NULL, "container_thread");

    return 0;
}
