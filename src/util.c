#include "util.h"

#include "queue.h"

static pthread_mutex_t pt_print_mutex;

void util_set_print_mutex(pthread_mutex_t mutex) {
    pt_print_mutex = mutex;
}

int str2hex(char* str, unsigned char* out, unsigned int* outlen, unsigned maxlen) {
	char* p = str;
	char high = 0, low = 0;
	int tmplen = strlen(p), cnt = 0;
	tmplen = strlen(p);
	while (cnt < (tmplen / 2) && cnt < maxlen)
	{
		high = ((*p > '9') && ((*p <= 'F') || (*p <= 'f'))) ? *p - 48 - 7 : *p - 48;
		low = (*(++p) > '9' && ((*p <= 'F') || (*p <= 'f'))) ? *(p)-48 - 7 : *(p)-48;
		out[cnt] = ((high & 0x0f) << 4 | (low & 0x0f));
		p++;
		cnt++;
	}
	if (tmplen % 2 != 0) out[cnt] = ((*p > '9') && ((*p <= 'F') || (*p <= 'f'))) ? *p - 48 - 7 : *p - 48;

	if (outlen != NULL) *outlen = tmplen / 2 + tmplen % 2;
	return tmplen / 2 + tmplen % 2;
}

void hex2str(char *pbDest, uint8_t *pbSrc, uint32_t nLen) {
    char ddl,ddh;
    int i;
    for (i=0; i<nLen; i++) {
        ddh = 48 + pbSrc[i] / 16;
        ddl = 48 + pbSrc[i] % 16;
        if (ddh > 57) ddh = ddh + 7;
        if (ddl > 57) ddl = ddl + 7;
        pbDest[i*2] = ddh;
        pbDest[i*2+1] = ddl;
    }
    pbDest[nLen*2] = '\0';
}

int print_stats_add(Pcoin c, Pconfig cfg, uint8_t device_id, uint8_t fpga_id, uint8_t type) {
    char *status;
    switch (type)
    {
    case 0:
        status = "valid ";
        c->all_stat.valid_nonce++;
        c->fpga_stats[fpga_id + device_id * cfg->fpga_per_device].valid_nonce++;
        break;
    case 1:
        status = "wrong ";
        c->all_stat.wrong_nonce++;
        c->fpga_stats[fpga_id + device_id * cfg->fpga_per_device].wrong_nonce++;
        break;
    case 2:
        status = "golden ";
        c->all_stat.golden_nonce++;
        c->fpga_stats[fpga_id + device_id * cfg->fpga_per_device].golden_nonce++;
        break;
    case 3:
        status = "accepted ";
        c->all_stat.accepted_nonce++;
        c->fpga_stats[fpga_id + device_id * cfg->fpga_per_device].accepted_nonce++;
        break;
    default:
        status = "";
        break;
    }

    //////
    // if (c->all_stat.golden_nonce > 1) {
    //     kill_miner();
    // }
    //////
    return printf("%shashboard: %d fpga: %d valid: %u, wrong: %u, golden: %u, accepted: %u\n",
        status, device_id, fpga_id,
        c->all_stat.valid_nonce,
        c->all_stat.wrong_nonce,
        c->all_stat.golden_nonce,
        c->all_stat.accepted_nonce
    );
}

int print_stats(Pcoin c, Pconfig cfg, uint8_t device_id, uint8_t fpga_id, uint8_t type) {
    char *status;
    switch (type)
    {
    case 0:
        status = "valid ";
        break;
    case 1:
        status = "wrong ";
        break;
    case 2:
        status = "golden ";
        break;
    case 3:
        status = "accepted ";
        break;
    default:
        status = "";
        break;
    }
    return printf("%sdevice: %d fpga: %d valid: %u, wrong: %u, golden: %u, accepted: %u\n",
        status, device_id, fpga_id,
        c->all_stat.valid_nonce,
        c->all_stat.wrong_nonce,
        c->all_stat.golden_nonce,
        c->all_stat.accepted_nonce
    );
}

int print_hex(uint8_t *in, unsigned length) {
    for (size_t i = 0; i < length; i++)
        printf("%02x", in[i]);
    printf("\n");
    return 0;
}

int print_regs(Preg preg, size_t reg_size) {
    // printf("\n\n%d\n\n", reg_size);
    for (size_t i = 0; i < reg_size; i++) {
        printf("Write %02x: %08x\n", preg[i].addr, __bswap_32(preg[i].content.u32));
    }
    return 0;
}

int write_regs_uart(int uart_fd, uint8_t fpga_id, Preg preg, size_t reg_size) {
    const size_t regs_per_burst = 16;
    const size_t bytes_per_reg = 6;
    size_t regs_written = 0;

    size_t buf_size = regs_per_burst * bytes_per_reg;
    uint8_t *buf = malloc(buf_size);
    do {
        for (size_t i = 0; i < regs_per_burst; i++) {
            buf[i * bytes_per_reg + 0] = 0x06 | fpga_id << 3;
            buf[i * bytes_per_reg + 1] = preg[regs_written + i].addr;
            for (size_t j = 0; j < bytes_per_reg - 2; j++)
                buf[i * bytes_per_reg + 2 + j] = preg[regs_written + i].content.u8[j];
        }

        size_t write_size = (reg_size - regs_written) > regs_per_burst ? buf_size : (reg_size - regs_written) * bytes_per_reg;

        // printf("write fpga_id: %d, \n", fpga_id);
        // print_hex(buf, write_size);

        ssize_t ret = write(uart_fd, buf, write_size);
        if (ret == -1) {
            return -1;
        }

        if ((reg_size - regs_written) > regs_per_burst)
            regs_written += regs_per_burst;
        else
            regs_written = reg_size;
        
        usleep(5000);/////////////////////////////////////////
    } while (regs_written < reg_size);

    free(buf);
    return 0;
}

int query_nonce_uart(Pconfig pcfg, Psubmit submit_info, int uart_fd) {
    size_t nonce_remain = 1;

    do {
        uint8_t uart_write_frame[6] = { 0x06 | (submit_info->fpga_id) << 3, 0x0b, 0x00, 0x00, 0x00, 0x01 };
        ssize_t ret = write(uart_fd, uart_write_frame, 6);
        if (ret == -1){
            printf("write error\n");
            return -1;
        }
        usleep(5000);/////////////////////////////////////////
        uint8_t uart_read_buf[9];
        ret = read(uart_fd, uart_read_buf, 9);
        if (ret == -1){
            printf("read error\n");
            return -1;
        }

        if (uart_read_buf[0] == 0)
            return 0;

        Psubmit psubmit;
        psubmit = malloc(sizeof(submit));
        
        memcpy(psubmit, submit_info, sizeof(submit));
        memcpy(&(psubmit->nonce), uart_read_buf + 1, 8);

        psubmit->nonce.u64 = __bswap_64(psubmit->nonce.u64);
        psubmit->nonce.u64 -= pcfg->coin_nonce_offset;
        psubmit->nonce.u64 = __bswap_64(psubmit->nonce.u64);

        // printf("submit deviceid: %d, fpga_id: %d, xnonce: %d\n", psubmit->device_id, psubmit->fpga_id, psubmit->xnonce);
        queue_put(pcfg->submit_queue, psubmit);
        
        nonce_remain = uart_read_buf[0];

        usleep(5000);/////////////////////////////////////////
    } while (nonce_remain == 0);

    return 0;
}

int print_submit(Psubmit ps) {
    printf("submit device %d fpga %d jobid %llx nonce %llx xnonce %x\n", ps->device_id, ps->fpga_id, ps->jobid, __bswap_64(ps->nonce.u64), ps->xnonce);
    return 0;
}
