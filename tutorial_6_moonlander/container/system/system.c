#include "bpf/bpfapi/helpers.h"
#include "../key_value_store.h"

#define DEBUG 0

#define REF_FACT 8
#define INIT_ALT (10000 << REF_FACT)  //m
// #define INIT_ALT (200000 << REF_FACT)  //m
#define INIT_VEL (1600 << REF_FACT) //m/s
#define INIT_ACC ((8 << REF_FACT)/5) //m/s^2 8/5=1.6

const char hdr_str[] = "Time\t Acc\t Vel\t Alt\t ";
const char val_str[] = "%d\t";
const char newline[] = "\n";

const char finish_str1[] = "Perfect landing.\n";
const char finish_str2[] = "Good landing.\n";
const char finish_str3[] = "Congratulation on a poor landing.\n";
const char finish_str4[] = "Impressive crash. Good luck making it back!\n";
const char finish_str5[] = "Crash landing. You have 5 hours of oxygen.\n";
const char finish_str6[] = "You blasted a new crater into the moon of %d meter depth.\n";


int64_t system(void) {
    uint32_t initFlag;
    uint32_t pauseFlag;
    bpf_fetch_global(SYS_INIT, &initFlag);
    bpf_fetch_global(SYS_PAUSE, &pauseFlag);
    
#if DEBUG
    bpf_printf(val_str, initFlag);
    bpf_printf(val_str, pauseFlag);
    bpf_printf(newline);
#endif //DEBUG

    uint32_t alt = INIT_ALT;
    uint32_t vel = INIT_VEL;
    uint32_t acc = INIT_ACC;
    uint32_t last_time = bpf_now_ms();
    
    if (initFlag == 0) {
        bpf_store_global(ALT, alt);
        bpf_store_global(VEL, vel);
        bpf_store_global(ACC, acc);
        bpf_store_global(LAST_TIME, last_time);
        bpf_store_global(SYS_INIT, 1);
        bpf_store_global(SYS_PAUSE, 1);
    } else if (pauseFlag == 0) {
        bpf_fetch_global(ALT, &alt);
        bpf_fetch_global(VEL, &vel);
        bpf_fetch_global(ACC, &acc);
        bpf_store_global(LAST_TIME, last_time);
        bpf_store_global(SYS_PAUSE, 1);
    } else {
        bpf_fetch_global(ALT, &alt);
        bpf_fetch_global(VEL, &vel);
        bpf_fetch_global(ACC, &acc);
        bpf_fetch_global(LAST_TIME, &last_time);

        uint32_t time_diff = ((((bpf_now_ms() - last_time) << REF_FACT) / 1000)) >> REF_FACT;

#if DEBUG
        bpf_printf(val_str, time_diff);
        bpf_printf(newline);
#endif //DEBUG 

        vel += acc * time_diff;
        // alt -= vel * time_diff;
        int diff = alt - vel * time_diff;

#if DEBUG
        bpf_printf(val_str, diff);
        bpf_printf(newline);
#endif //DEBUG

        if (diff > 0) {
            alt -= vel * time_diff;
        } else {
            alt = 0;
        }

        bpf_store_global(ALT, alt);
        bpf_store_global(VEL, vel);
        bpf_store_global(ACC, acc);
        bpf_store_global(LAST_TIME, bpf_now_ms());
    }
    
    if(alt != 0) {
        bpf_printf(hdr_str);
        bpf_printf(newline);
        bpf_printf(val_str, last_time);
        bpf_printf(val_str, (acc >> REF_FACT));
        bpf_printf(val_str, (vel >> REF_FACT));
        bpf_printf(val_str, (alt >> REF_FACT));
        bpf_printf(newline);
        return 1; 
    } else {
        bpf_printf(finish_str1);
        // if (vel <= 1) {
        //     bpf_printf(finish_str1);
        // } else if (vel <= 5) {
        //     bpf_printf(finish_str2);
        // } else if (vel <= 10) {
        //     bpf_printf(finish_str3);
        // } else if (vel <= 18) {
        //     bpf_printf(finish_str4);
        // } else if (vel <= 27) {
        //     bpf_printf(finish_str5);
        // } else {
        //     //uint32_t impact = (vel * 19 / 100) >> REF_FACT;
        //     //bpf_printf(finish_str6, impact);
        //     bpf_printf(finish_str5);
        // }
        return 0;
    }
}