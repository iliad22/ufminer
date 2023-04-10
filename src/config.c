#define HTTPSERVER_IMPL

#include <pthread.h>

#include "httpserver.h"
#include "cJSON.h"
#include "config.h"

static pthread_t g_pt_http_server;
static Pconfig g_cfg;

static void handle_request(struct http_request_s* request) {
    struct http_response_s* response = http_response_init();
    http_response_status(response, 200);
    http_response_header(response, "Content-Type", "text/plain");
    
    cJSON *json = cJSON_CreateObject();
    char num_of_fpga[4] = "";
    for (size_t i = 0; i < g_cfg->device_cnt * g_cfg->fpga_per_device; i++) {
        sprintf(num_of_fpga, "%02u", i);
        cJSON *fpga = cJSON_AddObjectToObject(json, num_of_fpga);
        cJSON_AddNumberToObject(fpga, "valid", g_cfg->m_coin->fpga_stats[i].valid_nonce);
        cJSON_AddNumberToObject(fpga, "wrong", g_cfg->m_coin->fpga_stats[i].wrong_nonce);
        cJSON_AddNumberToObject(fpga, "golden", g_cfg->m_coin->fpga_stats[i].golden_nonce);
        cJSON_AddNumberToObject(fpga, "accepted", g_cfg->m_coin->fpga_stats[i].accepted_nonce);
    }
    char *json_str = cJSON_Print(json);
    cJSON_Delete(json);

    http_response_body(response, json_str, strlen(json_str));
    http_respond(request, response);
    free(json_str);
}

static void* http_thread(void *arg) {
    struct http_server_s* server = http_server_init(8088, handle_request);
    if (server == NULL) { printf("failed to start http server.\n"); return arg; }
    http_server_listen(server);
    return arg;
}

Pconfig init_config(int argc, char *argv[]) {
    Pconfig pconfig = malloc(sizeof(config));
    if (pconfig == NULL) {
        return NULL;
    }
    memset(pconfig, 0, sizeof(config));

    g_cfg = pconfig;
    
    pconfig->algo_name = strdup(""); //free
    pconfig->device_name = strdup(""); //free

    
    pconfig->submit_queue = queue_create();
    if (pconfig->submit_queue == NULL) {
        return 0;
    }

    if(!strcmp(pconfig->device_name, "sk1")) {
        pconfig->baud_rate = SK1_DEFAULT_BAUDRATE;
        pconfig->device_cnt = SK1_MAX_DEVICES;
        pconfig->fpga_per_device = SK1_FPGA_PER_DEVICE;
        pconfig->fpga_cnt = SK1_MAX_DEVICES * SK1_FPGA_PER_DEVICE;
    } else if(!strcmp(pconfig->device_name, "f1")) {
        pconfig->baud_rate = F1_DEFAULT_BAUDRATE;
        pconfig->device_cnt = F1_MAX_DEVICES;
        pconfig->fpga_per_device = F1_FPGA_PER_DEVICE;
        pconfig->fpga_cnt = F1_MAX_DEVICES * F1_FPGA_PER_DEVICE;
    }

    // set random sequence
    srand((int)getpid());
    for (size_t i = 0; i < 64; i++)
        pconfig->random_sequence[i] = (uint8_t)(rand() % 255);

    int result;
    size_t pool_host_len;
    while ((result = getopt(argc, argv, "b:h:w:p:x:r:s:dz")) != -1) {
        switch (result) {
        case 'h':
            {
                size_t pool_host_len = strlen(optarg);
                pconfig->pool_host = malloc(pool_host_len + strlen("http:///") + 1);
                sprintf(pconfig->pool_host, "http://%s/", optarg);
            }
            break;
        case 'w':
            pconfig->wallet = strdup(optarg);
            break;
        case 'x':
            pconfig->socks5_host = strdup(optarg);
            break;
        case 'r':
            pconfig->rig_name = strdup(optarg);
            break;
        case 's':
            pconfig->password = strdup(optarg);
            break;
        case 'b':
            pconfig->baud_rate = atoi(optarg);
            if (pconfig->baud_rate < 0) {
                printf("invalid argument -b\n");
                return NULL;
            }
            break;
        case 'd':
            pconfig->network_debug_mode = true;
            printf("Network debug mode activated.\n");
            break;
        case 'z':
            for (size_t i = 0; i < 64; i++)
                pconfig->random_sequence[i] = 0;
            printf("Random sequence set to zero.\n");
            break;
        default:
            break;
        }
    }
    
    if (pconfig->wallet == 0 || pconfig->pool_host == 0) {
        // printf("usage: ./cfpgaminer -p [pool_host] -w [my_wallet]\n");
        return NULL;
    }
    if (pconfig->rig_name == 0) {
        pconfig->rig_name = strdup("rig0");
    }

    if (pconfig->password == 0) {
        pconfig->password = strdup("x");
    }
    
    printf("Pool: %s\nWallet: %s\n", pconfig->pool_host, pconfig->wallet);
    printf("Baudrate: %u\n", pconfig->baud_rate);
    
    // start http server
    int ret = pthread_create(&g_pt_http_server, NULL, http_thread, NULL);
    if (ret != 0) { printf("failed to start http server.\n"); return NULL; }

    ret = pthread_mutex_init(&(pconfig->global_print_mutex), NULL);
    if (ret != 0) { printf("failed to start mining client.\n"); return NULL;  }

    return pconfig;
}
