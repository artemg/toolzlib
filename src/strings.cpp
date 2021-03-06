#include "toolz/strings.h"
#include <toolz/log.h>
#include <wchar.h>
#include <ctype.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>


char     lower_buffer[CONVERT_BUFFER_SIZE];

void str_replace_char(char *str, char needle, char replace){
    while(*str != '\0'){
        if(*str == needle)
            *str = replace;
        str++;
    }
}

size_t str_replace_string(const char *input_str, const char *pattern, const char *replace,                                     
        char *out, size_t out_size)
{
    const char *it = input_str;                                                                                        
    size_t out_it = 0;
    size_t pattern_len = strlen(pattern);
    size_t replace_len = strlen(replace);                                                                              
    const char *pos = NULL;                                                                                            

    while( pos = strstr(it, pattern) ){                                                                                
        memcpy(out + out_it, it, pos - it);                                                                            
        out_it += (pos - it);                                                                                          
        memcpy(out + out_it, replace, replace_len);
        out_it += replace_len;
        it += (pos - it) + pattern_len;
    }
    out[out_it] = '\0';
    strncat(out, it, out_size - out_it);
    return 0;
}

char *str_rtrim(char *str, char c){
	if( str == NULL )
		return NULL;
    size_t len = strlen(str);
    while(len > 0){
        len--; //len = index now
        if( str[len] == c )
	{
            str[len] = '\0';
        } else {
            break;
        }
    }
    return str;
}

char *str_ltrim(char *str, char c){
	if( str == NULL )
		return NULL;
	while( *str == c && *str != '\0' )
		;
	return str;
}

char *str_trim(char *str, char c){
	return str_rtrim(str_ltrim(str, c), c);
}




void str_rtrim(char *str){
    size_t len = strlen(str);
    while(len > 0){
        len--; //len = index now
        if( str[len] == ' ' || 
            str[len] == '\t' ||
            str[len] == '\n' ||
            str[len] == '\r' ||
            str[len] == '\x0B'
        ){
            str[len] = '\0';
        } else {
            break;
        }
    }
}

size_t remove_ending_newline(char *str){
    size_t len = strlen(str);
    if( len > 0 && str[len - 1] == '\n'){
        str[len - 1] = '\0';
        len--;
    }
    return len;
}

char to_hex(char code) {
    static char hex[] = "0123456789abcdef";
    return hex[code & 15];
}

char from_hex(char ch) {
  return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

int url_encode(char *str, char *buf, size_t buf_len) {
    if(buf_len < (strlen(str) * 3 + 1)) {
        LOG(L_DEBUG, NULL, "Buffer is too small, at leest %ld bytes needed\n", 
            (strlen(str) * 3 +1));
        return -1;
    }
    char *pstr = str, *pbuf = buf;
    while (*pstr) {
        if (isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~') 
            *pbuf++ = *pstr;
        else if (*pstr == ' ') 
            *pbuf++ = '+';
        else 
            *pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4), *pbuf++ = to_hex(*pstr & 15);
        pstr++;
    }
    *pbuf = '\0';
    return 0;
}

int url_decode(char *str, char *buf, size_t buf_len) {
    if(buf_len < (strlen(str) + 1)) {
        LOG(L_DEBUG, NULL, "Buffer is too small, at leest %ld bytes needed\n", 
            (strlen(str) +1));
        return -1;
    }

    char *pstr = str, *pbuf = buf;
    while (*pstr) {
        if (*pstr == '%') {
            if (pstr[1] && pstr[2]) {
                *pbuf++ = from_hex(pstr[1]) << 4 | from_hex(pstr[2]);
                pstr += 2;
            }
        } else if (*pstr == '+') { 
            *pbuf++ = ' ';
        } else {
            *pbuf++ = *pstr;
        }
        pstr++;
    }
    *pbuf = '\0';
    return 0;
}

char *strtolower(char *s) {
    if(NULL == s) return NULL;
    int i;
    int n = strlen(s);
    for(i=0; i<n && i<CONVERT_BUFFER_SIZE-1; i++) {
        lower_buffer[i]= tolower(s[i]);
    }
    lower_buffer[++i] = '\0';
    return lower_buffer;
}

// http://www.openbsd.org/cgi-bin/cvsweb/~checkout~/src/lib/libc/string/strlcpy.c
// http://www.openbsd.org/cgi-bin/cvsweb/~checkout~/src/lib/libc/string/strlcat.c
size_t strlcpy(char *dst, const char *src, size_t siz)
{
	char *d = dst;
	const char *s = src;
	size_t n = siz;

	/* Copy as many bytes as will fit */
	if (n != 0) {
		while (--n != 0) {
			if ((*d++ = *s++) == '\0')
				break;
		}
	}

	/* Not enough room in dst, add NUL and traverse rest of src */
	if (n == 0) {
		if (siz != 0)
			*d = '\0';		/* NUL-terminate dst */
		while (*s++)
			;
	}

	return(s - src - 1);	/* count does not include NUL */
}

size_t strlcat(char *dst, const char *src, size_t siz)
{
	char *d = dst;
	const char *s = src;
	size_t n = siz;
	size_t dlen;

	/* Find the end of dst and adjust bytes left but don't go past end */
	while (n-- != 0 && *d != '\0')
		d++;
	dlen = d - dst;
	n = siz - dlen;

	if (n == 0)
		return(dlen + strlen(s));
	while (*s != '\0') {
		if (n != 1) {
			*d++ = *s;
			n--;
		}
		s++;
	}
	*d = '\0';

	return(dlen + (s - src));	/* count does not include NUL */
}

void hexdump (char *data, int len){
    int i, m = 0;
    unsigned char *buffer = NULL;
    unsigned char line[20];
    if (len <= 0){
        return;
    }
    buffer = (unsigned char *)malloc(len*10); // TODO NEED TO BE REWRITTEN !!!
    if (buffer == NULL){
        return;
    }
    memset(buffer, 0x00, len*10);
    for (i = 0, m = 0; i < len + (16 - (len % 16)); i++, m++){
        if (i < len){
            sprintf((char *)(buffer + strlen((const char*)buffer)), "%2.2x ", (unsigned int)data[i]);
            if (data[i] > 32 && data[i] < 127){
                line[m] = data[i];
            } else {
                line[m] = '.';
            }
        } else {
            sprintf((char *)(buffer + strlen((const char*)buffer)), "   ");
            line[m] = ' ';
        }
        if (i && ((i + 1) % 8 == 0)){
            sprintf((char *)(buffer + strlen((const char*)buffer)), " ");
        }
        if (i && ((i + 1) % 16 == 0)){
            line[m + 1] = '\00';
            m = -1;
            sprintf((char *)(buffer + strlen((const char*)buffer)), " %s\n", line);
        }
    }
    sprintf((char *)(buffer + strlen((const char*)buffer)), "\n");
    fprintf (stderr, "%s\n", buffer);
    free(buffer);
}

int CheckByMask(const char *wild, const char *string) {
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
