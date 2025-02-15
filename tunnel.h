#ifndef TUNNEL_H
#define TUNNEL_H

int tunnel_init(const char *address);

int tunnel_send(const unsigned char *str);

#endif
