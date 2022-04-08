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

#include <stdio.h>

#include "bpf.h"
#include "blob/container/consume/consume.bin.h"
#include "blob/container/produce/produce.bin.h"

#include "thread.h"
#include "sched.h"

#include "ztimer.h"
#include "timex.h"

static const uint32_t us = 10000;

void *producer(void *arg) {
    bpf_t * bpf = (bpf_t *) arg;

    uint64_t ctx = 0;
    int64_t result = 0;

    while (1) {
        // puts("Executing container hook 0...");
        bpf_execute_ctx(bpf, &ctx, sizeof(ctx), &result);
        // puts("Hook 0 executed.");
        ztimer_sleep(ZTIMER_USEC, us);
    }

    return NULL;
}

void *consumer(void *arg) {
    bpf_t * bpf = (bpf_t *) arg;

    uint64_t ctx = 0;
    int64_t result = 0;

    while (1) {
        // puts("Executing container hook 1...");
        bpf_execute_ctx(bpf, &ctx, sizeof(ctx), &result);
        // puts("Hook 1 executed.");
        ztimer_sleep(ZTIMER_USEC, us+100);    
    }
}

/* Pre-allocated stack for the virtual machine */
static uint8_t _stack_consume[512] = { 0 };
static uint8_t _stack_produce[512] = { 0 };

#ifndef WORKER_STACKSIZE
#define WORKER_STACKSIZE (THREAD_STACKSIZE_DEFAULT+THREAD_EXTRA_STACKSIZE_PRINTF)
#endif

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


    /* Create threads for containers */
    {
        static char stack[WORKER_STACKSIZE];
        thread_create(stack, sizeof(stack),
                    THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
                    producer, &bpf_produce, "producer");
    }

    {
        static char stack[WORKER_STACKSIZE];
        thread_create(stack, sizeof(stack),
                    THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
                    consumer, &bpf_consume, "consumer");    
    }

    return 0;
}
