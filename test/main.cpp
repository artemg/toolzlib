#include "test_config.h"

void log_(int level, void *arg, const char *format, ...){
	va_list va;
	va_start(va, format);
		vfprintf(stderr, format, va);
	va_end(va);
}

int main(int argc, char** argv){
    CTestConfig t;
    t.parse_commandline(argc, argv);

    return 0;
}


