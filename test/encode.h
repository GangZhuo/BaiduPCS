#ifndef _ENCODE_H
#define _ENCODE_H

int encode_is_utf8_sys();

int encode_to_utf8(char *dest, int sz, const char *src, int srcsz);
int encode_to_utf8_size(const char *src, int srcsz);

int encode_to_mbs(char *dest, int sz, const char *src, int srcsz);
int encode_to_mbs_size(const char *src, int srcsz);



#endif
