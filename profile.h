#ifndef COMMON_PROFILE_H
#define COMMON_PROFILE_H
#include <time.h>
#include <sys/time.h>
#include <stdint.h>
#include <map>
#include <string>
#include "log.h"
#include <vector>

using namespace std;


void start_profile();
void end_profile_print();
void end_profile_print(timeval *t);
double end_profile();
double diff_timeval(timeval *t1, timeval *t2);

struct profile_reporter_t{
    double t;
};

class ProfileReporter{
public:
    int addStat(const char *action, double time);
};

struct Eprofile{
    double time;
    int counter;
    bool operator<(const class Eprofile &right) const
    {
        return time < right.time;
    };
};

typedef map<string, Eprofile>::iterator stat_it;

class CProfilerTask{
public:
    void start(const char *str);
    void stop();
    void print(char *buf, uint64_t size); 
    ~CProfilerTask();
private:
    bool state_stoped;
    string name;
    struct timeval t;
    struct timeval t_e;
    static map<string, Eprofile> stat;
    static bool profile_enabled;
    typedef map<string, Eprofile>::iterator stat_it;
};

class FastProfiler {
    public:
        struct Eprofile{
            double time;
            int counter;
            Eprofile():
                time(0),
                counter(0)
            {};
        };

        FastProfiler();
        ~FastProfiler();
        typedef vector<Eprofile> FastProfileData;

        void start(int item);
        void stop();
        void print();

    private:
        static FastProfileData data_;
        struct timeval t1;
        struct timeval t2;
        bool started;
        int cur_index;
        static int size;
};

#endif
