#include "bpf/bpfapi/helpers.h"

static const char print_str[] = "Consume\n";

int consume(void)
{
   bpf_printf(print_str);
   return 0;
}
