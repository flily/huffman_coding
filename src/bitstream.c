#include "bitstream.h"

#include <string.h>

bincode_t* bincode_create(size_t size)
{
    bincode_t* code = (bincode_t*) malloc(sizeof(bincode_t));
    if (NULL == code)
        return NULL;
    
    code->size = size;
    code->length = 0;
    
    if (NULL == (code->bits = bincode_bit_alloc(size)))
    {
        free(code);
        return NULL;
    }
    
    return code;
}

uint8_t* bincode_bit_alloc(size_t length)
{
    uint8_t* bits = malloc(sizeof(uint8_t) * length);
    if (NULL == bits)
        return NULL;
    
    memset(bits, 0, length * sizeof(uint8_t));
    return bits;
}

int bincode_bit_realloc(bincode_t* code, size_t new_size)
{
    uint8_t* old_bits = NULL;
    uint8_t* new_bits = NULL;
    
    if (NULL == code)
        return -1;
    
    if (new_size <= code->size)
        return -2;
    
    if (NULL == (new_bits = bincode_bit_alloc(new_size)))
        return -3;
    
    old_bits = code->bits;
    if (NULL != old_bits)
    {
        memcpy(new_bits, old_bits, code->length);
        free(old_bits);
    }
    
    code->size = new_size;
    code->bits = new_bits;
    return 0;
}

int bincode_bit_append(bincode_t* code, int bit)
{
    if (NULL == code || NULL == code->bits)
        return -1;
    
    if (code->length == code->size)
    {
        size_t new_size = code->size + DEFAULT_BINCODE_SIZE;
        if (0 != bincode_bit_realloc(code, new_size))
            return -2;
    }
    
    code->bits[code->length] = bit;
    code->length += 1;
    return 0;
}

int bincode_bit_pop(bincode_t* code)
{
    if (NULL == code || NULL == code->bits)
        return -1;
    
    if (code->length > 0)
    {
        code->length -= 1;
        code->bits[code->length] = BINCODE_0;
    }
    
    return 0;
}

void bincode_free(bincode_t* code)
{
    if (NULL == code)
        return;
    
    if (NULL != code->bits)
        free(code->bits);
    
    free(code);
    return;
}

int bincode_get_string(bincode_t* code, char* buf, size_t bufsize)
{
    int i = 0;
    
    if (NULL == code)
        return 0;
    
    if (NULL == buf)
        return (int) code->length + 1;
    
    if (bufsize < code->length + 1)
        return -1;
    
    for (i = 0; i < code->length; i++)
    {
        if (BINCODE_0 == code->bits[i])
            buf[i] = '0';
        else
            buf[i] = '1';
    }
    
    buf[code->length] = 0;
    return (int) code->length + 1;
}

bitstream_t* bitstream_open(const char* filename, const char* mode)
{
    bitstream_t* bs = (bitstream_t*) malloc(sizeof(bitstream_t));
    
    if (NULL == bs)
        return NULL;
    
    bs->fd = fopen(filename, mode);
    bs->bit = 0;
    bs->bit_offset = 0;
    bs->eof = 0;
    return bs;
}

bitstream_t* bitstream_open_read(const char* filename)
{
    bitstream_t* bs = bitstream_open(filename, "r");
    int c = 0;
    
    if (NULL == bs)
        return NULL;
    
    bs->rw = BITSTREAM_READ;
    c = fgetc(bs->fd);
    if (EOF == c)
        bs->eof = 1;
    else
        bs->bit = c;
    
    return bs;
}

bitstream_t* bitstream_open_write(const char* filename)
{
    bitstream_t* bs = bitstream_open(filename, "w");
    
    if (NULL == bs)
        return NULL;
    
    bs->rw = BITSTREAM_WRITE;
    return bs;
}

int bitstream_get_bit(bitstream_t* bs)
{
    int bit = 0;
    
    if (NULL == bs || NULL == bs->fd)
        return BINCODE_ERR;
    
    if (bs->bit_offset < 0 || bs->bit_offset >= 8)
        return BINCODE_ERR;
    
    if (bs->eof)
        return BINCODE_ERR;
    
    bit = 0 == (bs->bit & bit_or_masks[bs->bit_offset]) ?
        BINCODE_0 : BINCODE_1;
    
    bs->bit_offset += 1;
    if (bs->bit_offset == 8)
    {
        int c = fgetc(bs->fd);
        if (EOF == c)
            bs->eof = 1;
        else
        {
            bs->bit = c;
            bs->bit_offset = 0;
        }
    }
    return bit;
}

int bitstream_set_bit(bitstream_t* bs, int bit)
{
    if (NULL == bs || NULL == bs->fd)
        return -1;
    
    if (bs->bit_offset < 0 | bs->bit_offset >= 8)
        return -2;
    
    if (BINCODE_0 == bit)
        bs->bit &= bit_and_masks[bs->bit_offset];
    
    else if (BINCODE_1 == bit)
        bs->bit |= bit_or_masks[bs->bit_offset];
    
    else
        return -3;
    
    bs->bit_offset += 1;
    if (bs->bit_offset >= 8)
    {
        if (0 != fputc(bs->bit, bs->fd))
            return -4;
        
        bs->bit = 0;
        bs->bit_offset = 0;
    }
    
    return 0;
}

int bitstream_write_bincode(bitstream_t* bs, bincode_t* code)
{
    int i = 0;
    
    if (NULL == code || NULL == code->bits)
        return -1;
    
    for (i = 0; i < code->length; i++)
    {
        int bit = code->bits[i];
        int ret = bitstream_set_bit(bs, bit);
        
        if (ret < 0)
            return ret;
    }
    
    return 0;
}

int bitstream_eof(bitstream_t* bs)
{
    if (NULL == bs || NULL == bs->fd)
        return 1;
    
    if (BITSTREAM_WRITE == bs->rw)
        return 0;
    
    if (0 != bs->eof)
        return 1;
        
    return 0;
}

void bitstream_close(bitstream_t* fd)
{
    if (NULL != fd)
    {
        if (NULL != fd->fd)
            fclose(fd->fd);
        
        free(fd);
    }
}

