#include <toolz/mem.h>
#include <malloc.h>
#include <memory>

size_t mallinfo_printf(char *buf, size_t buf_size){
    struct mallinfo mi = mallinfo();
    return snprintf(buf, buf_size, "%d %d %d %d %d %d %d %d %d %d",
            mi.arena,
            mi.ordblks,
            mi.smblks,
            mi.hblks,
            mi.hblkhd,
            mi.usmblks,
            mi.fsmblks,
            mi.uordblks,
            mi.fordblks,
            mi.keepcost
    );
}


template <typename T>
class mmap_allocator: public std::allocator<T>
{
    public:
        size_t alloced;

//        template<typename _Tp1>
//            struct rebind
//            {
//                typedef mmap_allocator<_Tp1> other;
//            };

        T *allocate(size_t n, const void *hint=0)
        {
            alloced += n;
            return std::allocator<T>::allocate(n, hint);
        }

        void deallocate(T *p, size_t n)
        {
            alloced -= n;
            return std::allocator<T>::deallocate(p, n);
        }

        mmap_allocator() throw(): std::allocator<T>() { alloced = 0; }
//        mmap_allocator(const mmap_allocator &a) throw(): std::allocator<T>(a) { }
//        ~mmap_allocator() throw() { }
};


#if 0
static char numa_available = numa_available();

struct lz_mem_tag_val_t{
    std::string name;
    size_t alloced;
};
static std::vector<lz_mem_tag_val_t> lz_mem_tag;

int lz_create_mem_tag(const char *name){
    lz_mem_tag_val_t t;
    t.name = name;  
    t.alloced = 0;
    lz_mem_tag.push_back(t);
    return lz_mem_tag.size() - 1;
}

void *lz_malloc(int tag, size_t size){
    void *ret;
    if( tag >= lz_mem_tag.size() )
        return NULL;
    ret = malloc(size);
    if( ret != NULL )
        lz_mem_tag[tag].alloced += size;
    return ret;
}

void lz_free(int tag, void *ptr){
    if( tag >= lz_mem_tag.size() )
        return;
    lz_mem_tag[tag].alloced -= size;
    free(ptr);
}

void *lz_malloc_numalocal(int tag, size_t size){
    if( !lz_numa_available )
        return lz_malloc(tag, size);
    return 
}
lz_free_(int tag);
#endif
