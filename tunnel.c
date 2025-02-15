#include <alsa/asoundlib.h>

#define TUNNEL_SEQ_STREAM_TYPE (SND_SEQ_OPEN_OUTPUT)
#define TUNNEL_SEQ_MODE (0)

#define TUNNEL_SEQ_PORT_CAP (SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SYNC_READ | SND_SEQ_PORT_CAP_SUBS_READ)
#define TUNNEL_SEQ_PORT_TYPE (SND_SEQ_PORT_TYPE_MIDI_GENERIC)

#define TUNNEL_EVENT_BUFSIZE (256)

static snd_seq_t *seq_handle = NULL;
static int src_port = -1;
static snd_midi_event_t *midi_event = NULL;
static snd_seq_event_t ev;

int tunnel_init(const char *address) {
    int ret = snd_seq_open(&seq_handle, "default", TUNNEL_SEQ_STREAM_TYPE, TUNNEL_SEQ_MODE); // 0 for blocking? Note double pointer
	if (ret < 0) {
		printf("error opening sequencer handle\n");
        return ret;
    }

    ret = snd_seq_set_client_name(seq_handle, "Harmidicaclient");
	if (ret < 0) {
		printf("error setting client name\n");
        return ret;
    }

	ret = snd_seq_create_simple_port(seq_handle, "Harmidicaport",
            TUNNEL_SEQ_PORT_CAP,
            TUNNEL_SEQ_PORT_TYPE);
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

    ret = snd_midi_event_new(TUNNEL_EVENT_BUFSIZE, &midi_event);
    if (ret < 0) {
		printf("error while allocating midi event\n");
        return ret;
    }

    return 0;
}

int tunnel_send(const unsigned char *buf, int size) {
    int ret = snd_midi_event_encode(midi_event, buf, size, &ev);
    if (ret < 0) {
		printf("error encoding midi event\n");
        return ret;
    }
    snd_seq_ev_set_source(&ev, src_port);
    snd_seq_ev_set_subs(&ev); // Send to all subscribers. Essentially this allows extra clients to listen
    snd_seq_ev_set_direct(&ev); // No queueing. Necessary even if using snd_seq_event_output_direct. Queueing =/= output buffering?
    return snd_seq_event_output_direct(seq_handle, &ev);
}
