#ifndef TOOLZLIB_TYPES_H
#define TOOLZLIB_TYPES_H

template <class T>
T *get_inactive_ptr(T *active_ptr, T *ptr1, T *ptr2){
    return (active_ptr == ptr1)? ptr2 : ptr1;
}

#endif
