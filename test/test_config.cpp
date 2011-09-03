#include "test_config.h"


void CTestConfig::help(CConfig *conf){
    conf->print_config_info();
    exit(0);
}
CTestConfig::CTestConfig(){
    addCallback("help", NULL, CTestConfig::help);
    addCallback1param("configfile", "c", CConfig::parseFilecb);
    addIntParam("zz", &z, 0);
}

void CTestConfig::parse_commandline(int argc, char** argv){
    CConfig::parse_commandline(argc, argv);
//    print_config_info();
}
