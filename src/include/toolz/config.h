#ifndef M_CONFIG_H
#define M_CONFIG_H

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <getopt.h>
#include <unistd.h>
#include <string>
#include <stdint.h>
#include <map>

#include "strings.h"

using namespace std;



/*
 *
 *
 *
 * TODO add callback functions
 *
 */

// config sources in incrementing priority
enum Econfig_param_source{ DEFAULT_VAL, CONFIG_FILE, COMMAND_LINE };

// used for print_config_info

class CConfig{
public:

    ~CConfig();

protected:
    int parsefile(const char *config_filename);
    int parse_comandline__(const char *default_conffilename, const char *version, int argc, char** argv);
    int parse_commandline(int32_t argc, char** argv);
    int addIntParam( const char *name, int32_t  *holder, const int32_t  default_value);
    int addUintParam(const char *name, uint32_t *holder, const uint32_t default_value);
    int addPCharParam(const char *name, char **holder,
            const char *default_value);
    int addUShortParam(const char *name, unsigned short *holder,
            const unsigned short default_value);
    int addInt64Param(const char *name, int64_t *holder,
            const int64_t default_value);
    int addDoubleParam(const char *name, double *holder,
            const double default_value);
    // check if str has '/' in the end, if not malloc() new str with ending '/' and free() str
    // return str if '/' exist in the end, or pointer to new malloc()'ed data with string
    void addEndingSlash(char **str);
    void make_full_path(const char *root_dir, const char *sub_dir, char **path);
    int clear_buffers();
    int print_config_info();
private:
    struct mymap_struct{
        string val;
        string source;
    };
    struct int_val{
        Econfig_param_source source;
        int32_t *holder;
    };
    map<string, int_val> int_map;
    struct uint_val{
        Econfig_param_source source;
        uint32_t *holder;
    };
    map<string, uint_val> uint_map;
    struct pchar_val{
        Econfig_param_source source;
        char **holder;
    };
    map<string, pchar_val> pchar_map;
    struct ushort_val{
        Econfig_param_source source;
        unsigned short *holder;
    };
    map<string, ushort_val> ushort_map;
    struct int64_val{
        Econfig_param_source source;
        int64_t *holder;
    };
    map<string, int64_val> int64_map;
    struct double_val{
        Econfig_param_source source;
        double *holder;
    };
    map<string, double_val> double_map;
};

#endif

