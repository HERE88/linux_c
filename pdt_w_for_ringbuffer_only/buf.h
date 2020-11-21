#ifndef _BUF_H
#define _BUF_H

#include <stddef.h>
#include <stdio.h>

 
struct rb_object
{
    size_t capacity;

    void *data;
    void *start;
    void *end;
};

typedef struct rb_object * ring_buffer;

ring_buffer buffer_init(size_t capacity);
void buffer_destroy(ring_buffer buffer);


size_t buffer_data_size(const ring_buffer buffer);
size_t buffer_space_available(const ring_buffer buffer);


size_t buffer_write(ring_buffer buffer, void *data, size_t data_size);
size_t buffer_read(ring_buffer buffer, void *dest, size_t data_size);


void buffer_clear(ring_buffer buffer);
void buffer_print_info(const ring_buffer buffer, FILE *stream);


#endif // _BUF_H
