#include "bpf/bpfapi/helpers.h"
#include "../key_value_store.h"

const char print_str[] = "Hello from fuel container\n";

int64_t fuel (void) {
    bpf_printf(print_str);
    return 1;
}