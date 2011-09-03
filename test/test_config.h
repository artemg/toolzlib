#include <toolz/config.h>

class CTestConfig : public CConfig {
public:
    CTestConfig();
    void parse_commandline(int argc, char **argv);
    static void help(CConfig *conf);

    int z;
};

