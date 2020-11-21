
#include "sys_file.h"
#include "buf.h"
#include "err_pub.h"


ring_buffer rb_ptr = NULL;

int main(int argc, char * argv[])
{  
    int ret = ERR_NONE;
    size_t ret_size;

    rb_ptr = buffer_init(8);
    if(! rb_ptr)
    {
        printf("buffer_init error!\n");
        return -1;
    }

    buffer_print_info(rb_ptr, stdout);

    ret_size = buffer_write(rb_ptr, "wanghaitao", strlen("wanghaitao"));
    printf("buffer_write ret:%zu!\n", ret_size);

    char temp[128];
    ret_size = buffer_read(rb_ptr, temp, 2);
    printf("buffer_read ret:%zu!\n", ret_size);
    
    buffer_print_info(rb_ptr, stdout);
    
    buffer_destroy(rb_ptr);
    return ret;
}
