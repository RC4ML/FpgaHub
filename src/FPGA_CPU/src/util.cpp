#include "util.hpp"

void set_cpu(thread& t,int cpu_index){
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(cpu_index, &cpuset);
	int rc = pthread_setaffinity_np(t.native_handle(),sizeof(cpu_set_t), &cpuset);
	if (rc != 0) {
		LOG_E("%-20s : %d","Error calling pthread_setaffinity_np",rc);
	}
}

char * time_string() {
  struct timespec ts;
  clock_gettime( CLOCK_REALTIME, &ts);
  struct tm * timeinfo = localtime(&ts.tv_sec);
  static char timeStr[60];
  sprintf(timeStr, "%.2d:%.2d:%.2d.%.3ld", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, ts.tv_nsec / 1000000);
  return timeStr;
}
