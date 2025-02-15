#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "tunnel.h"

#define WAIT(ticks) ({for (int i = 0; i < ticks; ++i);})
#define BUFSIZE 256

static int read_message(unsigned char *buf, int maxsize) {
    for (int i = 0; i < maxsize; ++i) {
        char next = (char)getchar();
        if (next != EOF && next != '\0' && next != '\n') {
            buf[i] = next;
        } else {
            buf[i] = '\0';
            return i;
        }
    }
    return 0;
}

// Pipe MIDI bytestream from stdin to specified ALSA sequencer
int main(int argc, char *argv[]) {
    if (argc < 2) {
		printf("Too few arguments\n");
        exit(1);
    }

    if (tunnel_init(argv[1])) {
        printf("Init error");
        exit(1);
    }

    unsigned char buf[BUFSIZE] = { 0 };
    while (1) {
        // Alternatively: snd_seq_event_output and snd_seq_drain_output
        // ...output_direct and ...drain_output are the only functions that actually send data
        int ret = read_message(buf, BUFSIZE);
        if (ret <= 0) {
            continue;
        }

        ret = tunnel_send(buf, ret);
        if (ret < 0) {
            printf("error sending event\n");
            exit(1);
        } else {
            printf("Output %d bytes\n", ret);
        }

        WAIT(1000000000);
    }

    return 0;
}
