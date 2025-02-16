#ifndef TUNNEL_H
#define TUNNEL_H

#include <alsa/asoundlib.h>

int tunnel_init(const char *address);

// Essentialy like snd_midi_event_encode, but reads from stdin directly instead of buf
int tunnel_read(snd_seq_event_t *ev);

int tunnel_send(snd_seq_event_t *ev);

#endif
