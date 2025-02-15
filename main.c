#include <stdio.h>
#include "tunnel.h"

#define WAIT(ticks) ({for (int i = 0; i < ticks; ++i);})

int main(int argc, char *argv[]) {
    if (argc < 2) {
		printf("Too few arguments\n");
        exit(1);
    }

    if (tunnel_init(argv[1])) {
        printf("Init error");
        exit(1);
    }

    while (1) {
        // Alternatively: snd_seq_event_output and snd_seq_drain_output
        // ...output_direct and ...drain_output are the only functions that actually send data
        int num = tunnel_send("\x90\x3c\x7f");
        if (num < 0) {
            printf("error sending event\n");
            exit(1);
        } else {
            printf("Output %d bytes\n", num);
        }

        WAIT(1000000000);
    }

    return 0;
}
