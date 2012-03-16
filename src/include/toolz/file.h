#ifndef TOOLZLIB_FILE_H
#define TOOLZLIB_FILE_H

int recursivly_mkdir(const char *filename);
int file_exist(const char *sfile);
int is_dir(const char *path);
int check_file_alligment(int fd, int alligment_size);
int file_copy(const char *src, const char *dst);
int rename_cross(const char *src, const char *dst);

#endif

