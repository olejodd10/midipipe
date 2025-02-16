#ifndef PIPE_H
#define PIPE_H

#include <alsa/asoundlib.h>

int pipe_init(const char *address);

// Essentialy like snd_midi_event_encode, but reads from stdin directly instead of buf
int pipe_read(snd_seq_event_t *ev);

int pipe_send(snd_seq_event_t *ev);

#endif
