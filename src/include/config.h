#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <pthread.h>

#include "queue.h"

typedef union taguint32_u {
    uint8_t u8[4];
    uint16_t u16[2];
    uint32_t u32;
}uint32_u;

typedef union taguint64_u {
    uint8_t u8[8];
    uint16_t u16[4];
    uint32_t u32[2];
    uint64_t u64;
}uint64_u;

typedef struct tagreg {
    uint8_t addr;
    uint32_u content;
}reg, *Preg;

typedef int (*common_block_calc_func)(Preg *reg_to_write, size_t *reg_size);
typedef int (*unique_block_calc_func)(uint8_t device_id, uint8_t fpga_id, uint8_t xnonce, Preg *reg_to_write, size_t *reg_size);

typedef struct tagstatistic {
    size_t valid_nonce;
    size_t wrong_nonce;
    size_t golden_nonce;
    size_t accepted_nonce;
}statistic, *Pstatistic;

typedef struct tagcoin {
    statistic all_stat;
    Pstatistic fpga_stats;
}coin, *Pcoin;

typedef struct tagdevice {
    // queue_t *submit_queue;
    // queue_t *job_queue;
    // queue_t *broadcast_reg_queue;
}device, *Pdevice;

typedef struct tagconfig {
    // general
    char *wallet;
    char *rig_name;
    char *password;
    char *algo_name;
    char *device_name;
    uint8_t random_sequence[64];
    pthread_mutex_t global_print_mutex;
    unique_block_calc_func unique_block_func;
    common_block_calc_func common_block_func;
    queue_t *submit_queue;
    struct tagjobid {
        uint64_t u64;
        char str[32];
        // size_t str_len;
    }jobid;

    // network
    char *socks5_host;
    uint16_t socks5_port;

    char *pool_host;
    uint16_t pool_port;

    // device
    size_t device_cnt;
    size_t fpga_cnt;
    size_t fpga_per_device;
    unsigned baud_rate;
    // uint8_t device_type;
    void *device;
    bool start_mining;
    Pdevice m_dev;

    // coin
    Pcoin m_coin;
    uint32_t coin_nonce_offset;

    //debug
    bool network_debug_mode;
}config, *Pconfig;


// typedef struct tagjob {
//     // uint8_t *write_buf;
//     Preg reg_to_write;
//     size_t write_len;
// }job, *Pjob;

typedef struct tagsubmit {
    uint64_u nonce;
    uint64_t jobid;
    uint8_t device_id;
    uint8_t fpga_id;
    uint8_t xnonce;
}submit, *Psubmit;

Pconfig init_config(int argc, char *argv[]);
void kill_miner();
