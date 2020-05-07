#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>

#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
#include<string.h>

void cp_file(char *src_file, char *dst_file)
{
   FILE *src, *dst;
   char buf[255];

   printf("src file: %s\n", src_file);
   printf("dst file: %s\n", dst_file);

   src = fopen(src_file, "r");  /* open for reading */
   dst = fopen(dst_file, "w");  /* truncate to 0 length or create for writing */
   if((NULL == src) || (NULL == dst))
   {
       printf("src/dst NULL error\n");
       exit(EXIT_FAILURE);
   }
   while(1)
   {
       if(fgets(buf, sizeof(buf), src))
           fputs(buf, dst);
       else
           break;
   }
   fclose(src);
   fclose(dst);
}

void cp_dir(char *src_path, char *dst_path){

    DIR *srcdir = opendir(src_path);
    if(NULL == srcdir)
    {
        printf("open dir %s error:%d, %s\n", src_path, errno, strerror(errno));
        return;
    }

    struct dirent *tmp = NULL;
    char src_dir_path[255];
    char dst_dir_path[255];

    while((tmp = readdir(srcdir)) != NULL)
    {
        if(strcmp(tmp->d_name, ".") == 0 ||
            strcmp(tmp->d_name, "..") == 0)
            continue;

        if(tmp->d_type == DT_DIR) /* Only copy regular files, not include directorys! */
            continue;

        memset(src_dir_path, 0, sizeof(src_dir_path));
        memset(dst_dir_path, 0, sizeof(dst_dir_path));
        snprintf(src_dir_path, sizeof(src_dir_path), "%s/%s", src_path, tmp->d_name);
        snprintf(dst_dir_path, sizeof(dst_dir_path), "%s/%s", dst_path, tmp->d_name);
        cp_file(src_dir_path, dst_dir_path);
    }

    closedir(srcdir);
}

int main(int argc, char **argv)
{
    cp_dir("/home/wanghaitao/tmp/linux_C/test_folder_s", "/home/wanghaitao/tmp/linux_C/test_folder_d");

    return 0;
}

