#include <stdio.h>
#include <alsa/asoundlib.h>

#define WAIT(ticks) ({for (int i = 0; i < ticks; ++i);})

int main(int argc, char *argv[]) {

    snd_seq_t *seq_handle = NULL;
	if (snd_seq_open(&seq_handle, "default", SND_SEQ_OPEN_OUTPUT, 0)) { // 0 for blocking? Note double pointer
		printf("error opening sequencer handle\n");
        exit(1);
    }
	
	if (snd_seq_set_client_name(seq_handle, "Harmidicaclient") < 0) {
		printf("error setting client name\n");
        exit(1);
    }

	int src_port = snd_seq_create_simple_port(seq_handle, "Harmidicaport",
                         SND_SEQ_PORT_CAP_READ | 
                         SND_SEQ_PORT_CAP_SYNC_READ | 
                         SND_SEQ_PORT_CAP_SUBS_READ ,
					     SND_SEQ_PORT_TYPE_MIDI_GENERIC);
    if (src_port < 0) {
		printf("error creating port\n");
        exit(1);
    }
	
    if (argc < 2) {
		printf("Too few arguments\n");
        exit(1);
    }
	snd_seq_addr_t dest_addr;
	if (snd_seq_parse_address(seq_handle, &dest_addr, argv[1]) < 0) {
		printf("invalid destination address\n");
		exit(1);
	}

    if (snd_seq_connect_to(seq_handle, src_port, dest_addr.client, dest_addr.port) < 0) {
		printf("error while subscribing\n");
		exit(1);
    }

    snd_midi_event_t *midi_event;
    if (snd_midi_event_new(256, &midi_event) < 0) {
		printf("error while allocating midi event\n");
		exit(1);
    }

    snd_seq_event_t ev;
    const unsigned char str[] =  "\x90\x3c\x7f";
    if (snd_midi_event_encode(midi_event, str, sizeof(str) - 1, &ev) < 0) {
		printf("error encoding midi event\n");
		exit(1);
    }
    snd_seq_ev_set_source(&ev, src_port); 
    snd_seq_ev_set_subs(&ev); // Send to all subscribers. Essentially this allows extra clients to listen
    snd_seq_ev_set_direct(&ev); // No queueing. Necessary even if using snd_seq_event_output_direct. Queueing =/= output buffering?

    while (1) {

        // Alternatively: snd_seq_event_output and snd_seq_drain_output
        // ...output_direct and ...drain_output are the only functions that actually send data
        int num = snd_seq_event_output_direct(seq_handle, &ev);
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
