check_PROGRAMS = t_config
t_config_SOURCES = main.cpp test_config.cpp test_config.h 
t_config_CPPFLAGS = $(DEPS_CFLAGS) -I ../src/include/
t_config_LDFLAGS = -L../src/ -ltoolz -lpthread -lrt
TESTS = $(check_PROGRAMS)
