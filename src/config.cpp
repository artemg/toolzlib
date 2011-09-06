#include "toolz/config.h"
#include <stdlib.h>

using namespace std;

void CConfig::printConfigcb(CConfig *conf){
    conf->print_config_info();
    exit(0);
}

void CConfig::parseFilecb(const char *fname, CConfig *conf){
    conf->parsefile(fname);
}

CConfig::~CConfig(){
    map<string, pchar_val>::const_iterator it;
    for(it=pchar_map.begin(); it!=pchar_map.end(); it++) {
       if(it->second.holder != NULL && *(it->second.holder) != NULL ) {
            free(*(it->second.holder));
       }
    }
}

int CConfig::parsefile(const char *config_filename)
{
    char buf[1024];
    const char str_delims[] = "=";
    char *strbuf_it = NULL;

    FILE *fp = fopen(config_filename, "r");
    if (fp == NULL){
        LOG(L_ERROR, NULL, "Cannot open file for read: '%s'\n", config_filename);
        return -1;
    }
    while (fgets(buf, sizeof(buf), fp) != NULL){
        remove_ending_newline(buf);
        if (!strlen(buf))    continue; // skip empty lines
        if (buf[0] == '\r')  continue; // skip empty lines
        if (buf[0] == '\n')  continue; // skip empty lines
        if (buf[0] == '#')   continue; // skip comments
        strbuf_it = buf;
        const char *key = strsep(&strbuf_it, str_delims);
        const char *value = strsep(&strbuf_it, str_delims);

        if( key == NULL || value == NULL ){
            LOG(L_WARN, NULL, "Wrong string '%s', while parsing config\n", buf);
            continue;
        }
        // find param in integer params
        {
            map<string, int_val>::iterator i = int_map.find(key);
            if (i != int_map.end() && i->second.source < CONFIG_FILE){
                *(i->second.holder) = atoi(value);
                i->second.source    = CONFIG_FILE;
                LOG(L_DEBUG12, NULL, "Config: added param '%s', value '%d'\n",
                    key, *(i->second.holder) );
                continue;
            }
        }
        // find param in unsigned integer params
        {
            map<string, uint_val>::iterator i = uint_map.find(key);
            if (i != uint_map.end() && i->second.source < CONFIG_FILE){
                *(i->second.holder) = (uint32_t)strtoul(value, NULL, 10);
                i->second.source    = CONFIG_FILE;
                LOG(L_DEBUG12, NULL, "Config: added param '%s', value '%d'\n",
                    key, *(i->second.holder) );
                continue;
            }
        }
        // find param in pchar params
        {
            map<string, pchar_val>::iterator i = pchar_map.find(key);
            if(i != pchar_map.end() && i->second.source < CONFIG_FILE){
                // enable this, when all deprecated functions will be removed
                //if( *(i->second.holder) != NULL ) free(*(i->second.holder));
                *(i->second.holder) = strdup(value);
                i->second.source    = CONFIG_FILE;
                LOG(L_DEBUG12, NULL, "Config: added param '%s', value '%s'\n",
                    key, *(i->second.holder) );
                continue;
            }
        }
        // find param in ushort params
        {
            map<string, ushort_val>::iterator i = ushort_map.find(key);
            if(i != ushort_map.end() && i->second.source < CONFIG_FILE){
                *(i->second.holder) = atoi(value);
                i->second.source    = CONFIG_FILE;
                LOG(L_DEBUG12, NULL, "Config: added param '%s', value '%d'\n",
                    key, *(i->second.holder) );
                continue;
            }
        }
        // find param in int64 params
        {
            map<string, int64_val>::iterator i = int64_map.find(key);
            if(i != int64_map.end() && i->second.source < CONFIG_FILE){
                *(i->second.holder) = strtoll(value, NULL, 10);
                i->second.source    = CONFIG_FILE;
                LOG(L_DEBUG12, NULL, "Config: added param '%s', value '%ld'\n",
                    key, *(i->second.holder) );
                continue;
            }
        }
        // find param in double params
        {
            map<string, double_val>::iterator i = double_map.find(key);
            if(i != double_map.end() && i->second.source < CONFIG_FILE){
                *(i->second.holder) = atof(value);
                i->second.source    = CONFIG_FILE;
                LOG(L_DEBUG12, NULL, "Config: added param '%s', value '%f'\n",
                    key, *(i->second.holder) );
                continue;
            }
        }
        LOG(L_DEBUG12, NULL, "Config string '%s' ignored\n",
            buf);
    }
    fclose(fp);
    return 0;
}

int CConfig::parse_commandline(int argc, char** argv)
{
    const size_t long_options_size = int_map.size() + pchar_map.size() \
        + ushort_map.size() + int64_map.size() + double_map.size() \
        + callback_map.size() + callback_1param_map.size() + 1;
    struct option *long_options = (struct option *) calloc(long_options_size, sizeof(struct option));
    size_t long_option_it = 0;

    //disable getopt error reporting
    opterr = 0;

    for(map<string, int_val>::iterator i = int_map.begin();
        i != int_map.end();
        ++i)
    {
        long_options[long_option_it].name    = i->first.c_str();
        long_options[long_option_it].has_arg = required_argument;
        long_options[long_option_it].flag    = NULL;
        long_options[long_option_it].val     = 0;
        ++long_option_it;
    }
    for(map<string, pchar_val>::iterator i = pchar_map.begin();
        i != pchar_map.end();
        ++i)
    {
        long_options[long_option_it].name    = i->first.c_str();
        long_options[long_option_it].has_arg = required_argument;
        long_options[long_option_it].flag    = NULL;
        long_options[long_option_it].val     = 0;
        ++long_option_it;
    }
    for(map<string, ushort_val>::iterator i = ushort_map.begin();
        i != ushort_map.end();
        ++i)
    {
        long_options[long_option_it].name    = i->first.c_str();
        long_options[long_option_it].has_arg = required_argument;
        long_options[long_option_it].flag    = NULL;
        long_options[long_option_it].val     = 0;
        ++long_option_it;
    }
    for(map<string, int64_val>::iterator i = int64_map.begin();
        i != int64_map.end();
        ++i)
    {
        long_options[long_option_it].name    = i->first.c_str();
        long_options[long_option_it].has_arg = required_argument;
        long_options[long_option_it].flag    = NULL;
        long_options[long_option_it].val     = 0;
        ++long_option_it;
    }
    for(map<string, double_val>::iterator i = double_map.begin();
        i != double_map.end();
        ++i)
    {
        long_options[long_option_it].name    = i->first.c_str();
        long_options[long_option_it].has_arg = required_argument;
        long_options[long_option_it].flag    = NULL;
        long_options[long_option_it].val     = 0;
        ++long_option_it;
    }
    for(map<string, callback_val>::iterator i = callback_map.begin();
        i != callback_map.end();
        ++i)
    {
        long_options[long_option_it].name    = i->first.c_str();
        long_options[long_option_it].has_arg = no_argument;
        long_options[long_option_it].flag    = NULL;
        long_options[long_option_it].val     = 0;
        ++long_option_it;
    }
    for(map<string, callback_1param_val>::iterator i = callback_1param_map.begin();
        i != callback_1param_map.end();
        ++i)
    {
        long_options[long_option_it].name    = i->first.c_str();
        long_options[long_option_it].has_arg = required_argument;
        long_options[long_option_it].flag    = NULL;
        long_options[long_option_it].val     = 0;
        ++long_option_it;
    }

    while(1){
        int option_index = 0;
        int option = getopt_long_only(argc, argv, "", long_options, &option_index);
        if( option == -1 ){
            // we want to use getopt later, so set it to 0
            optind = 0;
            break;
        }
        if( option == 0 ){ // long argument
            // find param in integer params
            {
                map<string, int_val>::iterator i = int_map.find(long_options[option_index].name);
                if (i != int_map.end() && i->second.source < COMMAND_LINE){
                    *(i->second.holder) = atoi(optarg);
                    i->second.source    = COMMAND_LINE;
                    LOG(L_DEBUG12, NULL, "Config: added param '%s', value '%d'\n",
                        long_options[option_index].name,
                         *(i->second.holder) );
                    continue;
                }
            }
            // find param in pchar params
            {
                map<string, pchar_val>::iterator i = pchar_map.find(long_options[option_index].name);
                if(i != pchar_map.end() && i->second.source < COMMAND_LINE){
                    // enable this, when all deprecated functions will be removed
                    //if( *(i->second.holder) != NULL ) free(*(i->second.holder));
                    *(i->second.holder) = strdup(optarg);
                    i->second.source    = COMMAND_LINE;
                    LOG(L_DEBUG12, NULL, "Config: added param '%s', value '%s'\n",
                        long_options[option_index].name,
                        *(i->second.holder) );
                    continue;
                }
            }
            // find param in ushort params
            {
                map<string, ushort_val>::iterator i = ushort_map.find(long_options[option_index].name);
                if(i != ushort_map.end() && i->second.source < COMMAND_LINE){
                    *(i->second.holder) = atoi(optarg);
                    i->second.source    = COMMAND_LINE;
                    LOG(L_DEBUG12, NULL, "Config: added param '%s', value '%d'\n",
                        long_options[option_index].name,
                        *(i->second.holder) );
                    continue;
                }
            }
            // find param in int64 params
            {
                map<string, int64_val>::iterator i = int64_map.find(long_options[option_index].name);
                if(i != int64_map.end() && i->second.source < COMMAND_LINE){
                    *(i->second.holder) = strtoll(optarg, NULL, 10);
                    i->second.source    = COMMAND_LINE;
                    LOG(L_DEBUG12, NULL, "Config: added param '%s', value '%ld'\n",
                        long_options[option_index].name,
                        *(i->second.holder) );
                    continue;
                }
            }
            // find param in double params
            {
                map<string, double_val>::iterator i = double_map.find(long_options[option_index].name);
                if(i != double_map.end() && i->second.source < COMMAND_LINE){
                    *(i->second.holder) = atof(optarg);
                    i->second.source    = COMMAND_LINE;
                    LOG(L_DEBUG12, NULL, "Config: added param '%s', value '%f'\n",
                        long_options[option_index].name,
                        *(i->second.holder) );
                    continue;
                }
            }
            // find param in callback params
            {
                map<string, callback_val>::iterator i = callback_map.find(long_options[option_index].name);
                if(i != callback_map.end() ){
                    (*(i->second.func))(this);
                    LOG(L_DEBUG12, NULL, "Config: called param '%s'\n",
                        long_options[option_index].name);
                    continue;
                }
            }
            // find param in callback_1param params
            {
                map<string, callback_1param_val>::iterator i = callback_1param_map.find(long_options[option_index].name);
                if(i != callback_1param_map.end() ){
                    (*(i->second.func))(optarg, this);
                    LOG(L_DEBUG12, NULL, "Config: called param '%s'\n",
                        long_options[option_index].name);
                    continue;
                }
            }
            LOG(L_DEBUG12, NULL, "Config string '%s' ignored\n",
                long_options[option_index].name);
        }
    }
    free(long_options);
}

void CConfig::addEndingSlash(char **str){
    const uint64_t str_len = strlen(*str);
    if ((*str)[str_len - 1] != '/'){
        // add '/' to the end
        const uint64_t size = str_len + 2;
        char *buf = (char *)malloc(size);
        lx_snprintf(buf, size, "%s/", *str);
        free(*str);
        *str = buf;
    }
}

void CConfig::make_full_path(const char *root_dir, const char *sub_dir, char **path){
    const uint64_t size = strlen(root_dir) + strlen(sub_dir) + strlen(*path)+1;
    char *buf = (char *) malloc(size);
    lx_snprintf(buf, size, "%s%s%s", root_dir, sub_dir, *path);
    free(*path);
    *path = buf;
}

// return 0 on SUCCESS
//        -1 if such name is already in map
int CConfig::addIntParam(const char *name, int32_t *holder_, const int32_t default_value)
{
    map<string, int_val>::const_iterator i = int_map.find(name);
    if (i != int_map.end() ) return -1;
    int_val x;
    x.source = DEFAULT_VAL;
    *holder_ = default_value;
    x.holder = holder_;
    int_map[name] = x;
    return 0;
}

// return 0 on SUCCESS
//        -1 if such name is already in map
int CConfig::addUintParam(const char *name, uint32_t *holder_, const uint32_t default_value)
{
    map<string, uint_val>::const_iterator i = uint_map.find(name);
    if (i != uint_map.end() ) return -1;
    uint_val x;
    x.source = DEFAULT_VAL;
    *holder_ = default_value;
    x.holder = holder_;
    uint_map[name] = x;
    return 0;
}

// return 0 on SUCCESS
//        -1 if such name is already in map
int CConfig::addPCharParam(const char *name, char **holder_, const char *default_value)
{
    map<string, pchar_val>::const_iterator i = pchar_map.find(name);
    if(i != pchar_map.end() ) return -1;
    pchar_val x;
    x.source = DEFAULT_VAL;
    *holder_ = (default_value != NULL) ? strdup(default_value) : NULL;
    x.holder = holder_;
    pchar_map[name] = x;
    return 0;
}

// return 0 on SUCCESS
//        -1 if such name is already in map
int CConfig::addUShortParam(const char *name, unsigned short *holder_,
    const unsigned short default_value)
{
    map<string, ushort_val>::const_iterator i = ushort_map.find(name);
    if(i != ushort_map.end() ) return -1;
    ushort_val x;
    x.source = DEFAULT_VAL;
    *holder_ = default_value;
    x.holder = holder_;
    ushort_map[name] = x;
    return 0;
}

// return 0 on SUCCESS
//        -1 if such name is already in map
int CConfig::addInt64Param(const char *name, int64_t *holder_,
    const int64_t default_value)
{
    map<string, int64_val>::const_iterator i = int64_map.find(name);
    if(i != int64_map.end() ) return -1;
    int64_val x;
    x.source = DEFAULT_VAL;
    *holder_ = default_value;
    x.holder = holder_;
    int64_map[name] = x;
    return 0;
}

// return 0 on SUCCESS
//        -1 if such name is already in map
int CConfig::addDoubleParam(const char *name, double *holder_,
    const double default_value)
{
    map<string, double_val>::const_iterator i = double_map.find(name);
    if(i != double_map.end() ) return -1;
    double_val x;
    x.source = DEFAULT_VAL;
    *holder_ = default_value;
    x.holder = holder_;
    double_map[name] = x;
    return 0;
}

// return 0 on SUCCESS
//        -1 if such name is already in map
int CConfig::addCallback(const char *name, const char *shortname, cb_ptr func)
{
    map<string, callback_val>::const_iterator i = callback_map.find(name);
    if(i != callback_map.end() ) return -1;
    callback_val x;
    x.func = func;
    callback_map[name] = x;
    return 0;
}

// return 0 on SUCCESS
//        -1 if such name is already in map
int CConfig::addCallback1param(const char *name, const char *shortname, cb_1param_ptr func)
{
    map<string, callback_1param_val>::const_iterator i = callback_1param_map.find(name);
    if(i != callback_1param_map.end() ) return -1;
    callback_1param_val x;
    x.func = func;
    callback_1param_map[name] = x;
    return 0;
}

int CConfig::clear_buffers()
{
    int_map.clear();
    pchar_map.clear();
    ushort_map.clear();
    int64_map.clear();
    double_map.clear();
    callback_map.clear();
    callback_1param_map.clear();
    return 0;
}

int CConfig::print_config_info()
{
    map<string, CConfig::mymap_struct> map_str;
    for(map<string, int_val>::const_iterator i = int_map.begin();
            i != int_map.end();
            ++i)
    {
        mymap_struct x;
        char buf[256];
        lx_snprintf(buf, sizeof(buf), "%d", *(i->second.holder));
        x.val        = buf;
        if( i->second.source == DEFAULT_VAL ){
            x.source = "default_val";
        }else if( i->second.source == CONFIG_FILE ){
            x.source = "config_file";
        }else if( i->second.source == COMMAND_LINE ){
            x.source = "command_line";
        }
        map_str[i->first] = x;
    }

    for(map<string, pchar_val>::const_iterator i = pchar_map.begin();
            i != pchar_map.end();
            ++i)
    {
        mymap_struct x;
        x.val        = (*(i->second.holder) == NULL) ?  "NULL" : *(i->second.holder);
        if( i->second.source == DEFAULT_VAL ){
            x.source = "default_val";
        }else if( i->second.source == CONFIG_FILE ){
            x.source = "config_file";
        }else if( i->second.source == COMMAND_LINE ){
            x.source = "command_line";
        }
        map_str[i->first] = x;
    }

    for(map<string, ushort_val>::const_iterator i = ushort_map.begin();
            i != ushort_map.end();
            ++i)
    {
        mymap_struct x;
        char buf[256];
        lx_snprintf(buf, sizeof(buf), "%u", *(i->second.holder));
        x.val        = buf;
        if( i->second.source == DEFAULT_VAL ){
            x.source = "default_val";
        }else if( i->second.source == CONFIG_FILE ){
            x.source = "config_file";
        }else if( i->second.source == COMMAND_LINE ){
            x.source = "command_line";
        }
        map_str[i->first] = x;
    }

    for(map<string, int64_val>::const_iterator i = int64_map.begin();
            i != int64_map.end();
            ++i)
    {
        mymap_struct x;
        char buf[256];
        lx_snprintf(buf, sizeof(buf), "%ld", *(i->second.holder));
        x.val        = buf;
        if( i->second.source == DEFAULT_VAL ){
            x.source = "default_val";
        }else if( i->second.source == CONFIG_FILE ){
            x.source = "config_file";
        }else if( i->second.source == COMMAND_LINE ){
            x.source = "command_line";
        }
        map_str[i->first] = x;
    }

    for(map<string, double_val>::const_iterator i = double_map.begin();
            i != double_map.end();
            ++i)
    {
        mymap_struct x;
        char buf[256];
        lx_snprintf(buf, sizeof(buf), "%f", *(i->second.holder));
        x.val        = buf;
        if( i->second.source == DEFAULT_VAL ){
            x.source = "default_val";
        }else if( i->second.source == CONFIG_FILE ){
            x.source = "config_file";
        }else if( i->second.source == COMMAND_LINE ){
            x.source = "command_line";
        }
        map_str[i->first] = x;
    }
    // now make it pretty =)
    // find max length
    uint16_t max_len_1 = 0, max_len_2 = 0, max_len_3 = 0;
    for(map<string, mymap_struct>::const_iterator i = map_str.begin();
            i != map_str.end();
            ++i)
    {
        if( max_len_1 < i->first.size() )
            max_len_1 = i->first.size();
        if( max_len_2 < i->second.val.size() )
            max_len_2 = i->second.val.size();
        if( max_len_3 < i->second.source.size() )
            max_len_3 = i->second.source.size();
    }
    // отступы
    max_len_1 += 2;
    max_len_2 += 2;
    max_len_3 += 2;

    // head line
    for(int i=0; i<max_len_1+max_len_2+max_len_3; ++i)
        printf("=");
    printf("\n");
    // head
    printf("%-*s%-*s%-*s\n",
            max_len_1, "parametr",
            max_len_2, "value",
            max_len_3, "source");
    // hrad line
    for(int i=0; i<max_len_1+max_len_2+max_len_3; ++i)
        printf("=");
    printf("\n");
    // data
    for(map<string, mymap_struct>::const_iterator i = map_str.begin();
            i != map_str.end();
            ++i)
    {
        printf("%-*s%-*s%-*s\n",
            max_len_1, i->first.c_str(),
            max_len_2, i->second.val.c_str(),
            max_len_3, i->second.source.c_str());
    }
    // bottom line
    for(int i=0; i<max_len_1+max_len_2+max_len_3; ++i)
        printf("=");
    printf("\n");
    return 0;
}

