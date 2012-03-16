#include "toolz/file.h"
#include "toolz/log.h"

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int recursivly_mkdir(const char *filename){
    int32_t  err = -1, i = 0;
    char buf[1024];

    if (filename == NULL ){
        goto fail;
    }
    memset(buf, 0x00, sizeof(buf));
    while (filename[i] != '\0'){
        if (filename[i] == '/'){
            if (i >= (sizeof(buf) - 1)){
                LOG(L_ERROR, "", "Temp buffer overflow: '%d'\n", i);
                goto fail;
            }
            memcpy(buf, filename, i + 1);
            mkdir(buf, 0777);
        }
        i++;
    }
    err = 0;
ret:
    return err;
fail:
    err = -1;
    goto ret;
}

int file_exist(const char *sfile){
    struct stat oFileInfo;
    int istat = stat(sfile, &oFileInfo);
    if(istat == 0) {
        return 1;
    }
    return 0;
}

int is_dir(const char *path){
    struct stat st;
    if (!(lstat(path, &st)>=0)) return 0;
    if (st.st_mode&S_IFDIR) {
        return 1;
    }
    return 0;
}

int check_file_alligment(int fd, int alligment_size){
    struct stat st;
    if( fstat(fd, &st) ){
        LOG(L_WARN, "", "Fstat of fd=%d failed\n", fd);
    }
    return (st.st_size % alligment_size);
}

int file_copy(const char *src, const char *dst){
    int ret;
    struct stat st;
    char buf[4096];
    ssize_t read_ret;
    int fd;
    FILE *fp = fopen(src, "r");
    if( fp == NULL ){
        LOG(L_WARN, "", "Cant open file %s\n", src);
        return -1;
    }
    if( recursivly_mkdir(dst) ){
        return -1;
    }

    while(1){
        fd = open(dst, O_WRONLY | O_CREAT | O_EXCL, 0777);
        if( fd != -1 ){
            break;
        } else { //fd == -1
            if( errno != EEXIST ){
                LOG(L_WARN, "", "cant open file '%s'\n", dst);
            }
        }
        if( remove(dst) ){
            LOG(L_WARN, "", "Cant remove '%s'\n", dst);
            return -1;
        }
    }
    FILE *out = fdopen(fd, "w");
    if( out == NULL ){
        LOG(L_WARN, "", "Cant open file %s\n", dst);
        fclose(fp);
        return -1;
    }
    if( fstat(fileno(fp), &st) ){
        LOG(L_WARN, "", "Fstat failed\n");
        ret = -1;
        goto ret_close;
    }
    // sendfile doesnt work :( 
    /*
    if( sendfile(fileno(out), fileno(fp), 0, st.st_size) != st.st_size ){
        ERROR_LOG(L_WARN, "Sendfile failed\n");
        return -1;
    }
    */
    while((read_ret = fread(buf, 1, sizeof(buf), fp)) != 0 ){
        if( fwrite(buf, read_ret, 1, out) != 1 ){
            LOG(L_WARN, "", "Write failed\n");
            ret = -1;
            goto ret_close;
        }
    }
    ret = 0;
ret_close:
    if( fclose(fp) )
        return -1;
    if( fclose(out) )
        return -1;
    return ret;
}

int rename_cross(const char *src, const char *dst){
    if( file_copy(src, dst) ){
        LOG(L_WARN, "", "file_copy failed\n");
        return -1;
    }
    if( remove(src) ){
        LOG(L_WARN, "", "remove failed\n");
        return -1;
    }
    return 0;
}

