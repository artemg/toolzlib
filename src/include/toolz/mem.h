#ifndef TOOLZ_MEM_H
#define TOOLZ_MEM_H

size_t mallinfo_printf(char *buf, size_t buf_size);

int lz_create_mem_tag(const char *name);

void *lz_malloc(int tag, size_t size);
lz_free(void *ptr, int tag);

void *lz_malloc_numalocal(int tag, size_t size);
lz_free_(int tag);

#endif
