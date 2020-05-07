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

void cp_dir(char *src_path, char *dst_path)
{
    DIR *srcdir = opendir(src_path);
    if(NULL == srcdir)
    {
        printf("open dir %s error:%d, %s\n", src_path, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    struct dirent *tmp = NULL;

    /* What to do if end with '/'??[wanghaitao_20200114] */
    char src_dir_path[255];
    size_t src_length = strlen(src_path);
    memset(src_dir_path, '\0', sizeof(src_dir_path));
    strcpy(src_dir_path, src_path);
    src_dir_path[src_length] = '/';
    src_dir_path[src_length + 1] = '\0';

    char dst_dir_path[255];
    size_t dstlen = strlen(dst_path);
    memset(dst_dir_path, '\0', sizeof(dst_dir_path));
    strcpy(dst_dir_path, dst_path);
    dst_dir_path[dstlen] = '/';
    dst_dir_path[dstlen + 1] = '\0';

    char *dirname = strrchr(src_path, '/');
    printf("dirname %s\n", dirname + 1); /* Must point to the next character after '/'![wanghaitao] */
    strcat(dst_dir_path, dirname + 1);
    int dst_pre_path_length = strlen(dst_dir_path);
    mkdir(dst_dir_path, S_IRUSR|S_IWUSR|S_IXUSR);

    while(1)
    {
        tmp = readdir(srcdir);
        if(NULL == tmp) break;
        if(strcmp(".", tmp->d_name) == 0 ||
            strcmp("..", tmp->d_name) == 0)
            continue;
        /* get source path (file or director) */
        strcat(src_dir_path, tmp->d_name);
        if(tmp->d_type ==  DT_DIR) /* copy director */
        {
            printf("sub dir name is: %s\n", tmp->d_name);
            cp_dir(src_dir_path, dst_dir_path);  //recursive!
        }
        else /* copy file */
        {
            /* set dst file path */
            strcat(dst_dir_path, "/");
            strcat(dst_dir_path, tmp->d_name);
            printf("src: %s \ndst: %s\n", src_dir_path, dst_dir_path);
            cp_file(src_dir_path, dst_dir_path);
            dst_dir_path[dst_pre_path_length] = '\0'; /* retrurn to the previous state. */
        }
        /* Reset prefix of source path (file or director) */
        src_dir_path[src_length + 1] = '\0';
    }
    closedir(srcdir);
}

int main(int argc, char **argv)
{
#if 0
    if(3 != argc)
    {
        printf("USAGE: %s src_dir, dst_disr\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    cp_dir(argv[1], argv[2]);
    //cp_file(argv[1], argv[2]);
#else
    cp_dir("/home/wanghaitao/tmp/linux_C/test_folder2/test_folder3", "/home/wanghaitao/tmp/linux_C");
#endif

    return 0;
}
