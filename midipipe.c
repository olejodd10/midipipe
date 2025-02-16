#include "midipipe.h"

#include <alsa/asoundlib.h>
#include <stdio.h>

#define MIDIPIPE_SEQ_STREAM_TYPE (SND_SEQ_OPEN_OUTPUT)
#define MIDIPIPE_SEQ_MODE (0)

#define MIDIPIPE_SEQ_PORT_CAP (SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SYNC_READ | SND_SEQ_PORT_CAP_SUBS_READ)
#define MIDIPIPE_SEQ_PORT_TYPE (SND_SEQ_PORT_TYPE_MIDI_GENERIC)

#define MIDIPIPE_EVENT_BUFSIZE (256)

#define MIDIPIPE_CLIENT_NAME "MIDIPipe client"
#define MIDIPIPE_PORT_NAME "MIDIPipe port"

static snd_seq_t *seq_handle = NULL;
static int src_port = -1;
static snd_midi_event_t *midi_event = NULL;
static snd_seq_event_t ev;

int midipipe_init(const char *address) {
    int ret = snd_seq_open(&seq_handle, "default", MIDIPIPE_SEQ_STREAM_TYPE, MIDIPIPE_SEQ_MODE); // 0 for blocking? Note double pointer
	if (ret < 0) {
		printf("error opening sequencer handle\n");
        return ret;
    }

    ret = snd_seq_set_client_name(seq_handle, MIDIPIPE_CLIENT_NAME);
	if (ret < 0) {
		printf("error setting client name\n");
        return ret;
    }

	ret = snd_seq_create_simple_port(seq_handle,
            MIDIPIPE_PORT_NAME,
            MIDIPIPE_SEQ_PORT_CAP,
            MIDIPIPE_SEQ_PORT_TYPE);
    if (ret < 0) {
		printf("error creating port\n");
        return ret;
    }
    src_port = ret;

	snd_seq_addr_t dest_addr;
    ret = snd_seq_parse_address(seq_handle, &dest_addr, address);
	if (ret < 0) {
		printf("invalid destination address\n");
        return ret;
	}

    ret = snd_seq_connect_to(seq_handle, src_port, dest_addr.client, dest_addr.port);
    if (ret < 0) {
		printf("error while subscribing\n");
        return ret;
    }

    ret = snd_midi_event_new(MIDIPIPE_EVENT_BUFSIZE, &midi_event);
    if (ret < 0) {
		printf("error while allocating midi event\n");
        return ret;
    }

    return 0;
}

int midipipe_read(snd_seq_event_t *ev) {
    while (1) {
        int ret = getchar();
        if (ret < 0) {
            printf("error or EOF while reading byte\n");
            return ret;
        } else {
            char c = (unsigned char)ret;
            ret = snd_midi_event_encode_byte(midi_event, c, ev);
            if (ret < 0) {
                printf("error encoding byte\n");
                snd_midi_event_reset_encode(midi_event);
                return ret;
            } else if (ret == 1) {
                // Event has been completely encoded
                return 0;
            }
        }
    }
}

int midipipe_send(snd_seq_event_t *ev) {
    snd_seq_ev_set_source(ev, src_port);
    snd_seq_ev_set_subs(ev); // Send to all subscribers. Essentially this allows extra clients to listen
    snd_seq_ev_set_direct(ev); // No queueing. Necessary even if using snd_seq_event_output_direct. Queueing =/= output buffering?
    return snd_seq_event_output_direct(seq_handle, ev);
}
