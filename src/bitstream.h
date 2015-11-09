#ifndef ___huffman__bitstream_h___
#define ___huffman__bitstream_h___

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define DEFAULT_BINCODE_SIZE 256

#define BINCODE_ERR  (-1)
#define BINCODE_0    0
#define BINCODE_1    1

struct bincode_s
{
    size_t size;
    size_t length;
    uint8_t* bits;
};

typedef struct bincode_s bincode_t;

bincode_t* bincode_create(size_t size);

uint8_t* bincode_bit_alloc(size_t length_in_byte);

int bincode_bit_realloc(bincode_t* code, size_t new_size);

int bincode_bit_append(bincode_t* code, int bit);

int bincode_bit_pop(bincode_t* code);

void bincode_free(bincode_t* code);

int bincode_get_string(bincode_t* code, char* buf, size_t bufsize);

static uint8_t bit_and_masks[8] =
{
    0x7f, 0xbf, 0xdf, 0xef, 0xf7, 0xfb, 0xfd, 0xfe
};

static uint8_t bit_or_masks[8] =
{
    0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01
};

#define BITSTREAM_READ  0
#define BITSTREAM_WRITE 1

struct bitstream_s
{
    FILE* fd;
    int bit;
    int bit_offset;
    int rw;
    int eof;
};

typedef struct bitstream_s bitstream_t;

bitstream_t* bitstream_open_read(const char* filename);

bitstream_t* bitstream_open_write(const char* filename);

int bitstream_get_bit(bitstream_t* bs);

int bitstream_set_bit(bitstream_t* bs, int bit);

int bitstream_write_bincode(bitstream_t* bs, bincode_t* code);

int bitstream_eof(bitstream_t* bs);

void bitstream_close(bitstream_t* fd);

#endif
