#ifndef MIDIPIPE_H
#define MIDIPIPE_H

#include <alsa/asoundlib.h>

int midipipe_init(const char *address);

// Essentialy like snd_midi_event_encode, but reads from stdin directly instead of buf
int midipipe_read(snd_seq_event_t *ev);

int midipipe_send(snd_seq_event_t *ev);

#endif
