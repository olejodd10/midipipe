#include <unistd.h>
#include <stdio.h>
#include <alsa/asoundlib.h>

#include "pipe.h"

// Pipe MIDI bytestream from stdin to specified ALSA sequencer
int main(int argc, char *argv[]) {
    if (argc < 2) {
		printf("Too few arguments\n");
        return 1;
    }

    if (pipe_init(argv[1])) {
        printf("Init error");
        return 1;
    }

    while (1) {
        // Alternatively: snd_seq_event_output and snd_seq_drain_output
        // ...output_direct and ...drain_output are the only functions that actually send data
        snd_seq_event_t ev;
        int ret = pipe_read(&ev);
        if (ret < 0) {
            // Ignore errors and try again
            continue;
        }

        ret = pipe_send(&ev);
        if (ret < 0) {
            printf("error sending event\n");
        } else {
            printf("Output %d bytes\n", ret);
        }
    }

    return 0;
}
