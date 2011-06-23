#include "profile.h"
#include "strings.h"

#ifndef CLOCK_MONOTONIC_RAW
#define CLOCK_MONOTONIC_RAW      4
#endif

static struct timeval t1,t2;
map<string, Eprofile> CProfilerTask::stat;

bool CProfilerTask::profile_enabled = false;

void start_profile(){
    clock_gettime(CLOCK_MONOTONIC_RAW, &t1);
}

void end_profile_print(){
    clock_gettime(CLOCK_MONOTONIC_RAW, &t2);
    double d1 = double(t1.tv_sec) + double(t1.tv_usec ) / 1000000.0;
    double d2 = double(t2.tv_sec) + double(t2.tv_usec ) / 1000000.0;    
    LOG(L_DEBUG, NULL, "Elapsed time: %fs.", d2-d1);
    
    char buf[20];
    time_t t;
    struct tm *tm;
    time(&t);
    tm = localtime(&t);
    strftime(buf, 20, "%F %T", tm);
    LOG(L_DEBUG, NULL, " (%s)\n", buf);
}

void end_profile_print(timeval *t_start){
    struct timeval t_end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &t_end);
    double d1 = double(t_start->tv_sec) + double(t_start->tv_usec ) / 1000000.0;
    double d2 = double(t_end.tv_sec) + double(t_end.tv_usec ) / 1000000.0;    
    LOG(L_DEBUG, NULL, "Elapsed time: %fs.", d2-d1);
    
    char buf[20];
    time_t t;
    struct tm *tm;
    time(&t);
    tm = localtime(&t);
    strftime(buf, 20, "%F %T", tm);
    LOG(L_DEBUG, NULL, " (%s)\n", buf);
}

double end_profile(){
    clock_gettime(CLOCK_MONOTONIC_RAW, &t2);
    double d1 = double(t1.tv_sec) + double(t1.tv_usec ) / 1000000.0;
    double d2 = double(t2.tv_sec) + double(t2.tv_usec ) / 1000000.0;
    return d2 - d1;
}

double diff_timeval(timeval *t1, timeval *t2){
    double d1 = double(t1->tv_sec) + double(t1->tv_usec ) / 1000000.0;
    double d2 = double(t2->tv_sec) + double(t2->tv_usec ) / 1000000.0;
    return d2 - d1;
}


void CProfilerTask::start(const char *str){
    if(profile_enabled) {
        state_stoped = false;
        name = str;
        clock_gettime(CLOCK_MONOTONIC_RAW, &t);
    }
}

void CProfilerTask::stop(){
    if(profile_enabled) {
        stat_it it;

        clock_gettime(CLOCK_MONOTONIC_RAW, &t_e);
        double d1 = double(t.tv_sec)   + double(t.tv_usec )   / 1000000.0;
        double d2 = double(t_e.tv_sec) + double(t_e.tv_usec ) / 1000000.0;
        const double time_el = d2 - d1;
        it = stat.find(name);
        if( it != stat.end() ){
            it->second.counter++;
            it->second.time += time_el;
        } else {
            Eprofile x;
            x.counter = 1;
            x.time = time_el;
            stat[name] = x;
        }
        state_stoped = true;
    }
}

void CProfilerTask::print(char *buf, uint64_t size){
    if(profile_enabled) {
        char tmpbuf[4096];
        map<Eprofile, string> print_data;
        typedef map<Eprofile, string>::iterator print_data_it;
        // sort by totatl time
        // print name, total time, count, totatl time/count. Sort by total time
        for(stat_it it = stat.begin();
                it != stat.end();
                ++it)
        {
            print_data[it->second] = it->first;
        }

        static const char *sep = "name ==== time    ====   count    ==   time/count\t===============================\n";
        const int sep_size = strlen(sep) + 1;
        lx_strncat(buf, sep, size);
        size -= sep_size;
        for(print_data_it it = print_data.begin();
                it != print_data.end();
                ++it)
        {
            double tmp = (it->first.counter==0)? 0 :it->first.time / it->first.counter;
            int ret = lx_snprintf(tmpbuf, sizeof(tmpbuf), "%s\t%f\t%d\t%f\n",
                    it->second.c_str(),
                    it->first.time,
                    it->first.counter,
                    tmp);
            lx_strncat(buf, tmpbuf, size);
            size -= ret;
        }
        lx_strncat(buf, sep, size);
        size -= sep_size;
    }
}

CProfilerTask::~CProfilerTask(){
        if( !state_stoped )
            stop();
}

FastProfiler::FastProfiler() {
    data_.reserve(1000);
    started = false;
    cur_index = 0;
}

FastProfiler::~FastProfiler() {
    stop();
}

void FastProfiler::start(int item) {
    if(started) {
        stop();
    }
    if(item+1 > size) {
        size = item + 1;
    }
    cur_index = item;
    clock_gettime(CLOCK_MONOTONIC_RAW, &t1);
    started = true;
}

void FastProfiler::stop() {
    clock_gettime(CLOCK_MONOTONIC_RAW, &t2);
    double d1 = double(t1.tv_sec) + double(t1.tv_usec ) / 1000000.0;
    double d2 = double(t2.tv_sec) + double(t2.tv_usec ) / 1000000.0;
    data_[cur_index].time += d2 - d1;
    data_[cur_index].counter +=1;
    started = false;
}

void FastProfiler::print() {
    char tmpbuf[1024];
    static const char *sep = "name ==== time    ====   count    ==   time/count\t===============================\n";
    for(int i=0; i<size; i++) {
        double tmp = (data_[i].counter==0)? 0 : data_[i].time / data_[i].counter;
        printf("%d\t%f\t%d\t%f\n",
                i,
                data_[i].time,
                data_[i].counter,
                tmp);
    }

}

FastProfiler::FastProfileData FastProfiler::data_;
int FastProfiler::size = 0;

int ProfileReporter::addStat(const char *action, double time){
    return 0;
}

#ifdef TEST_LIB

int main(){
    char buf[10000];
    CProfilerTask t;
    {
    CProfilerTask t2;
    t2.start("a");
    for(int i=0; i<10; ++i){
        t.start("me");
        sleep(1);
        t.stop();
    }
//    t2.stop();
    }
    t.print(buf, sizeof(buf));
    printf("%s\n", buf);
    return 0;
}

#endif
