#include "bpf/bpfapi/helpers.h"

static const char print_str[] = "Produce\n";

void produce(void)
{
    bpf_printf(print_str);
}
