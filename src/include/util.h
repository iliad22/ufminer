#pragma once

#include <stdint.h>
#include <stdio.h>

#include "config.h"

int print_hex(uint8_t *in, unsigned length);
void util_set_print_mutex(pthread_mutex_t mutex);

void hex2str(char *pbDest, uint8_t *pbSrc, uint32_t nLen);
int str2hex(char* str, unsigned char* out, unsigned int* outlen, unsigned maxlen);

int print_stats(Pcoin c, Pconfig cfg, uint8_t device_id, uint8_t fpga_id, uint8_t type);
int print_stats_add(Pcoin c, Pconfig cfg, uint8_t device_id, uint8_t fpga_id, uint8_t type);

int print_regs(Preg preg, size_t reg_size);
int print_submit(Psubmit ps);
int write_regs_uart(int uart_fd, uint8_t fpga_id, Preg preg, size_t reg_size);
int query_nonce_uart(Pconfig pcfg, Psubmit submit_info, int uart_fd);