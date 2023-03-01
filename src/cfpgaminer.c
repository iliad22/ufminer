#include "config.h"
#include "util.h"

// coins
#include "radiant.h"

// devices
#include "f1.h"s
#include "sk1.h"

static bool miner_killed = false;

void kill_miner() {
    // todo
    printf("miner killed.\n");
    miner_killed = true;
}

int main(int argc, char *argv[]) {
    Pconfig pconfig;
    pconfig = init_config(argc, argv);
    if (pconfig == NULL) {
        return 0;
    }

    Pdevice Pdevice;
    Pdevice = init_f1(pconfig);
    // Pdevice = init_sk1(pconfig);
    if (Pdevice == NULL) {
        return 0;
    }

    Pcoin pcoin;
    pcoin = init_radiant(pconfig, Pdevice);
    if (pcoin == NULL) {
        return 0;
    }

    while(1) {
        usleep(1000);
    }
    return 0;
}
