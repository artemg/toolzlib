#ifndef COMMON_STRINGS_H
#define COMMON_STRINGS_H

#include <string.h>
#include <wchar.h>
#include <wctype.h>
#include <iconv.h>
#include <locale.h>
#include <stdarg.h>
#include <string.h>

#include "log.h"

#define LOCALE_CHARSET          "UTF-8"
#define CONVERT_BUFFER_SIZE     32768

// todo 
struct str_t {
    size_t len;
    unsigned char *data;
};

char *utf8_lowercase(char *str);
char *utf8_lowercase_with_replace(char *str);
char *utf8_lowercase_with_replace_my(char *str);

void str_replace(char *str, char needle, char replace);
size_t str_replace(const char *input_str, const char *pattern, const char *replace, char *out, size_t out_size);

void str_rtrim(char *str);
// remove new line character in the end of string, return strlen of string
size_t remove_ending_newline(char *str);


// libc 2.0 ret < 0
// libc 2.1 - needed size - 1

// lx_snprintf_func(char *str, size_t size, const char *format, ...)
#define lx_snprintf(args...) lx_snprintf_func(__FILE__, __LINE__, args)
inline int lx_snprintf_func(const char *file, const int line, char *str, size_t size, const char *format, ...){
    va_list va;
    va_start(va, format);
    int ret = vsnprintf(str, size, format, va);
    va_end(va);
    if( ret < 0 || (size_t)ret >= size ){
        LOG(L_ERROR, NULL, "Buffer too small. File:%s Line:%d Buffer size=%lu, needed=%d\n",
            file, line, size, ret+1);
    }
    return ret;
}

// WARNING! This function is not libc strncat compatible
// 3rd argument is destination size, not N (bumber of bytes to write)
inline char *lx_strncat(char *dest, const char *src, size_t dest_size){
    size_t dest_len = strlen(dest);
    size_t i;

    for (i = 0 ; i < (dest_size - dest_len - 1)  && src[i] != '\0' ; i++)
        dest[dest_len + i] = src[i];
    dest[dest_len + i] = '\0';
    if( src[i] != '\0' ){
        LOG(L_WARN, NULL, "Strncat need more buffer, bufsize = %ld\n", dest_size);
    }

    return dest;
}

/*----------------------------------------------------------------------*/
/* Сpавнение стpоки с шаблоном. В шаблоне можно yпотpеблять знаки '?'   */
/* (любой знак) и '*' (любое количество любых знаков)                   */
inline int CheckByMask(const char *wild, const char *string) {
  const char *cp = NULL, *mp = NULL;

  while ((*string) && (*wild != '*')) {
    if ((*wild != *string) && (*wild != '?')) {
      return 0;
    }
    wild++;
    string++;
  }

  while (*string) {
    if (*wild == '*') {
      if (!*++wild) {
        return 1;
      }
      mp = wild;
      cp = string+1;
    } else if ((*wild == *string) || (*wild == '?')) {
      wild++;
      string++;
    } else {
      wild = mp;
      string = cp++;
    }
  }

  while (*wild == '*') {
    wild++;
  }
  return !*wild;
}

char *url_encode(char *str, char *buf, size_t buf_len);
char *url_decode(char *str, char *buf, size_t buf_len);
char *strtolower(char *s);

// http://www.openbsd.org/cgi-bin/cvsweb/~checkout~/src/lib/libc/string/strlcpy.c
// http://www.openbsd.org/cgi-bin/cvsweb/~checkout~/src/lib/libc/string/strlcat.c
size_t strlcpy(char *dst, const char *src, size_t siz);
size_t strlcat(char *dst, const char *src, size_t siz);

void hexdump(char *data, int len);

#endif
