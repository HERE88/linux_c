#include "buf.h"

#include <stdlib.h>
#include <string.h>


ring_buffer buffer_init(size_t capacity)
{
    /* doesn't make sense */
    if (capacity == 0)
        return (ring_buffer) 0;
    ring_buffer buffer = (ring_buffer) malloc(sizeof(struct rb_object));
    if (! buffer)
        return (ring_buffer) 0;

    buffer->capacity = capacity;

    /* Always_keep_one_slot_open */
    buffer->data = malloc(capacity + 1);

    if (! buffer->data)
    {
        free(buffer);
        return (ring_buffer) 0;
    }

    /* end == begin, so buffer is empty */
    buffer->start = buffer->data;
    buffer->end = buffer->data;
    return buffer;
}


void buffer_destroy(ring_buffer buffer)
{
    if (! buffer)
        return;
    free(buffer->data);
    free(buffer);
    /* just in case */
    buffer = (ring_buffer) 0;
}


size_t buffer_data_size(const ring_buffer buffer)
{
    if (buffer->end >= buffer->start)
        return buffer->end - buffer->start;
    return (buffer->end + buffer->capacity + 1) - buffer->start;
}


size_t buffer_space_available(const ring_buffer buffer)
{
    return buffer->capacity - buffer_data_size(buffer);
}


size_t buffer_write(ring_buffer buffer, void *data, size_t data_size)
{
    size_t buf_space = buffer_space_available(buffer);
    size_t wr_size = (data_size > buf_space) ? buf_space : data_size;

    size_t end_free_segment;
    if (buffer->end >= buffer->start)
    {
        end_free_segment = buffer->data + buffer->capacity - buffer->end
                         + (buffer->start == buffer->data ? 0 : 1);
        if (end_free_segment >= wr_size) /* Simple case */
        {
            memcpy(buffer->end, data, wr_size);
            buffer->end += wr_size;
        }
        else
        {
            memcpy(buffer->end, data, end_free_segment);
            memcpy(buffer->data, data + end_free_segment,
                 wr_size - end_free_segment);
            buffer->end = buffer->data + wr_size - end_free_segment;
        }
    }
    else
    {
        memcpy(buffer->end, data, wr_size);
        buffer->end += wr_size;
    }
    return wr_size;
}


size_t buffer_read(ring_buffer buffer, void *dest, size_t data_size)
{
    size_t buf_data = buffer_data_size(buffer);
    size_t rd_size = (data_size > buf_data) ? buf_data : data_size;

    size_t end_data_segment;
    if (buffer->end >= buffer->start)
    {
        memcpy(dest, buffer->start, rd_size);
    }
    else
    {
        end_data_segment = buffer->data + buffer->capacity - buffer->start
                         + (buffer->start == buffer->data ? 0 : 1);
        if (end_data_segment >= rd_size) /* Simple case */
        {
            memcpy(dest, buffer->start, rd_size);
        }
        else
        {
            memcpy(dest, buffer->start, end_data_segment);
            memcpy(dest + end_data_segment, buffer->data,
                 rd_size - end_data_segment);
        }
    }

    buffer->start += rd_size;
    if (buffer->start > buffer->data + buffer->capacity)
        buffer->start -= buffer->capacity;
    return rd_size;
}


void buffer_clear(ring_buffer buffer)
{
    buffer->start = buffer->data;
    buffer->end = buffer->data;
}


void buffer_print_info(const ring_buffer buffer, FILE *stream)
{
    fprintf(stream, "-------- RING BUFFER ----------\n");
    fprintf(stream, "Capacity:   %zu\n", buffer->capacity);
    fprintf(stream, "Data size:  %zu\n", buffer_data_size(buffer));
    fprintf(stream, "Free space: %zu\n", buffer_space_available(buffer));
    fprintf(stream, "Begin:      %ld\n", buffer->start - buffer->data);
    fprintf(stream, "End:        %ld\n", buffer->end - buffer->data);
}
