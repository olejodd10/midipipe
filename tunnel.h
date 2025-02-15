#ifndef TUNNEL_H
#define TUNNEL_H

int tunnel_init(const char *address);

// str must be null-terminated. The characters leading up to this must be a valid midi event
int tunnel_send(const unsigned char *buf, int size);

#endif
