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
#include "blob/container/consume/consume.bin.h"
#include "blob/container/produce/produce.bin.h"

#include "thread.h"
#include "xtimer.h"
#include "timex.h"

char container_0_thread_stack[THREAD_STACKSIZE_MAIN];
char container_1_thread_stack[THREAD_STACKSIZE_MAIN];

void *container_thread_0(void *arg) {
    bpf_t * bpf = (bpf_t *) arg;

    uint64_t ctx = 0;
    int64_t result = 0;
    while (1) {
        puts("Executing container hook 0...");
        bpf_execute_ctx(bpf, &ctx, sizeof(ctx), &result);
        puts("Hook 0 executed.");
        xtimer_sleep(N);
    }

    return NULL;
}

void *container_thread_1(void *arg) {
    bpf_t * bpf = (bpf_t *) arg;

    uint64_t ctx = 0;
    int64_t result = 0;

    while (1) {
        puts("Executing container hook 1...");
        // bpf_hook_execute(BPF_HOOK_TRIGGER_NETIF, NULL, sizeof(NULL), NULL);
        bpf_execute_ctx(bpf, &ctx, sizeof(ctx), &result);
        puts("Hook 1 executed.");
        xtimer_sleep(N);        
    }
}

/* Pre-allocated stack for the virtual machine */
static uint8_t _stack_consume[512] = { 0 };
static uint8_t _stack_produce[512] = { 0 };

int main(void)
{
    /* Initialize the bpf subsystem */
    bpf_init();

    puts("All up, running the Femto-Container application now");

    /* Define the containers */
    bpf_t bpf_consume = {
        .application = consume_bin,             /* The increment.bin content */
        .application_len = sizeof(consume_bin), /* Length of the application */
        .stack = _stack_consume,                /* Preallocated stack */
        .stack_size = sizeof(_stack_consume),   /* And the length */
    };

    bpf_setup(&bpf_consume);

    bpf_t bpf_produce = {
        .application = produce_bin,             /* The increment.bin content */
        .application_len = sizeof(produce_bin), /* Length of the application */
        .stack = _stack_produce,                /* Preallocated stack */
        .stack_size = sizeof(_stack_produce),   /* And the length */
    };

    bpf_setup(&bpf_produce);


    // bpf_execute_ctx(&bpf_produce, NULL, sizeof(NULL), NULL);
    // int res = bpf_execute_ctx(&bpf, &ctx, sizeof(ctx), &result);

    // /* Connect containers to two different hooks*/
    // bpf_hook_t hook_consume = {
    //     .application = &bpf_consume,
    // };

    // bpf_hook_install(&hook_consume, BPF_HOOK_TRIGGER_NETIF); 
    

    // bpf_hook_t hook_produce = {
    //     .application = &bpf_produce,
    // };

    // bpf_hook_install(&hook_produce, BPF_HOOK_SCHED);



    /* Create threads for containers */
    thread_create(container_0_thread_stack, sizeof(container_0_thread_stack),
                  THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
                  container_thread_0, &bpf_produce, "container_thread_0");

    thread_create(container_1_thread_stack, sizeof(container_1_thread_stack),
                  THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
                  container_thread_1, &bpf_consume, "container_thread_1");

    return 0;
}
