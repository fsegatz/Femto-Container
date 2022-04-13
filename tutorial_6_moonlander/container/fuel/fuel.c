#include "bpf/bpfapi/helpers.h"
#include "../key_value_store.h"

const char val_str[] = "%d\t\t";
const char newline[] = "\n";

int64_t fuel (uint32_t *ctx) {
    uint32_t fuel_rate = *ctx;
    
#if DEBUG
    bpf_printf(val_str, fuel_rate);
    bpf_printf(newline);
#endif //DEBUG

    bpf_store_global(FUEL_RATE, (fuel_rate << REF_FACT));
    bpf_store_global(SYS_PAUSE, 0);
    uint32_t alt;
    bpf_fetch_global(ALT, &alt);
    if(alt == 0) {
        return 0;
    } else {
        return 1;
    }
}