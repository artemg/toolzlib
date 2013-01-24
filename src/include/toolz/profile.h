#ifndef COMMON_PROFILE_H
#define COMMON_PROFILE_H
#include <time.h>
#include <sys/time.h>
#include <stdint.h>
#include <map>
#include <string>
#include "log.h"
#include <vector>
#include <pthread.h>

using namespace std;
// 5 min
#define AVG_REQUESTS_SWITCH_PERIOD 300

void start_profile();
void end_profile_print();
void end_profile_print(timespec *t);
double end_profile();
double diff_timeval(timeval *t1, timeval *t2);
double diff_timespec(timespec *t1, timespec *t2);




// thread safe
class ProfileReporter{
public:
    ProfileReporter();
    ~ProfileReporter();
    struct status_val{
        int hitcount;
        int time_sum;
        double min_exec_time;
        double max_exec_time;
        double avg_exec_time;
        uint64_t more_1sec_exec_time;
        uint64_t prev_period_req;
        uint64_t cur_period_req;
        int period_start_time;
    };
    typedef map<string, status_val> stat_t;
    int addStat(const char *action, double time);
    int getStat(stat_t *stat);
private:
    stat_t stat;
    pthread_rwlock_t stat_lock;
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
    struct timespec t;
    struct timespec t_e;
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
        struct timespec t1;
        struct timespec t2;
        bool started;
        int cur_index;
        static int size;
};

struct status_t{
    uint64_t hitcount;
    int time_sum;
    double min_exec_time;
    double max_exec_time;
    double avg_exec_time;
    uint64_t more_1sec_exec_time;
    uint64_t prev_period_req;
    uint64_t cur_period_req;
    int period_start_time;
};

int update_statistic(status_t *stat, double exec_time); 

#endif
