#include <stdio.h>

#include "bpf.h"
#include "blob/container/system/system.bin.h"
#include "blob/container/fuel/fuel.bin.h"

#include "sched.h"
#include "thread.h"
#include "ztimer.h"

#define WORKER_STACKSIZE (THREAD_STACKSIZE_DEFAULT+THREAD_EXTRA_STACKSIZE_PRINTF)

/* Pre-allocated stack for the virtual machine */
static uint8_t stack_system[512] = { 0 };
static uint8_t stack_fuel[512] = { 0 };

static void *fuel_thread(void *arg) {
    bpf_t *bpf = (bpf_t *) arg;
    uint32_t ctx = 0;
    int64_t result = 1;

    while (result) {
        //puts("Executing container hook 1...");
        printf("Enter fuel rate: ");
        scanf("%u", &ctx);
        int err = bpf_execute_ctx(bpf, &ctx, sizeof(ctx), &result);
        if(err) {
            printf("Errorcode: %d\n", err);
        }
        ztimer_sleep(ZTIMER_USEC, 10000000);
    }
    return NULL;
}

static void *system_thread(void *arg) {
    bpf_t * bpf = (bpf_t *) arg;
    bpf->flags |= BPF_CONFIG_NO_RETURN; //bugfix

    uint64_t ctx = 0;
    int64_t result = 1;

    while (result) {
        //puts("Executing container hook 0...");
        int err = bpf_execute_ctx(bpf, &ctx, sizeof(ctx), &result);
        if(err) {
            printf("Errorcode: %d\n", err);
        }
        // puts("Hook 0 executed.");
        ztimer_sleep(ZTIMER_USEC, 500000);
    }
    
    return NULL;
}

int main(void) {
     /* Initialize the bpf subsystem */
    bpf_init();

    puts("All up, running the Femto-Container application now");

    /* Define the containers */
    bpf_t bpf_system = {
        .application = system_bin,              /* The system.bin content */
        .application_len = sizeof(system_bin),  /* Length of the application */
        .stack = stack_system,                  /* Preallocated stack */
        .stack_size = sizeof(stack_system),     /* And the length */
    };
    bpf_setup(&bpf_system);

    bpf_t bpf_fuel = {
        .application = fuel_bin,                /* The fuel.bin content */
        .application_len = sizeof(fuel_bin),    /* Length of the application */
        .stack = stack_fuel,                    /* Preallocated stack */
        .stack_size = sizeof(stack_fuel),       /* And the length */
    };
    bpf_setup(&bpf_fuel);

    /* Create threads for containers */
    {
        static char stack[WORKER_STACKSIZE];
        thread_create(stack, sizeof(stack),
                    THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
                    system_thread, &bpf_system, "system");
    }
    {
        static char stack[WORKER_STACKSIZE];
        thread_create(stack, sizeof(stack),
                    THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST,
                    fuel_thread, &bpf_fuel, "fuel");
    }

    return 0;
}