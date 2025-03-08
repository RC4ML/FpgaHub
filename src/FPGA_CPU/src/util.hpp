#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <stdio.h>
#include <string>
#include <infiniband/verbs.h>
#include <sys/mman.h>
#include <thread>
#include <assert.h>
#include <numaif.h>
#include <numa.h>
#include <unistd.h>

using namespace std;

#define DEBUG
#define INFO

#define __FILENAME__ (strrchr(__FILE__, '/') + 1)

#ifdef DEBUG
#define LOG_D(format, ...) \
{char buf[60];\
snprintf(buf,60,"[DEBUG][%s][%s:%d][%s]",time_string(), __FILENAME__,  __LINE__, __FUNCTION__ );\
printf("%-60s" format "\n",buf, ##__VA_ARGS__);}
#else
#define LOG_D(format, ...)
#endif

#ifdef INFO
#define LOG_I(format, ...)  \
{char buf[60];\
snprintf(buf,60,"[INFO][%s][%s:%d][%s]",time_string(), __FILENAME__,  __LINE__, __FUNCTION__ );\
printf("%-60s" format "\n",buf, ##__VA_ARGS__);}
#else
#define LOG_I(format, ...)
#endif

#define LOG_E(format, ...) fprintf(stderr, "[ERROR][%s][%s:%d][%s]: " format "\n",time_string(), __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__);\
exit(1);

void set_cpu(thread& t,int cpu_index);

char * time_string();
#endif