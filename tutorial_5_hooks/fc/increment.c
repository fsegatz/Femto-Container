#include "bpf/bpfapi/helpers.h"

static const char print_str_b[] = "Test_before\n";
static const char print_str[] = "Test\n";
static const char print_str_a[] = "Test_after\n";

void increment(unsigned long *context)
{
    unsigned long c = 0;
    bpf_printf(print_str_b);
    while (c++ < 100)
    {
        bpf_printf(print_str);
    }
    bpf_printf(print_str_a);
}