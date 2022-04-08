#include "bpf/bpfapi/helpers.h"

#define SHARED_KEY 0x50

static const char print_str[] = "Consume: %d\n";

void consume(void)
{
   uint32_t value;
   bpf_fetch_global(SHARED_KEY, &value);

   if (value > 0) {
      value--;
   }
   
   bpf_store_global(SHARED_KEY, value);

   bpf_printf(print_str, value);
}
