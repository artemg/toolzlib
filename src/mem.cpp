#include <toolz/mem.h>
#include <malloc.h>
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
