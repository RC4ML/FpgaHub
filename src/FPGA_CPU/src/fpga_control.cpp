#include "fpga_control.hpp"

void qp_init(volatile uint32_t *bar, uint32_t credit){
	bar[103] = credit;
	bar[101] = 0;
	bar[101] = 1;
	usleep(1000);
	bar[101] = 0;
	bar[101] = 1;
	usleep(1000);
}

void reset(volatile uint32_t *bar){
	bar[102] = 0;//global start
	bar[100] = 1;
	usleep(1000);
	bar[100] = 0;//reset
	usleep(1000);
}

void print_latency(volatile uint32_t *bar,int index_high,int index_low,string str, int num){
	printf("%s:\n",str.c_str());
	uint64_t latency = bar[index_low] + ((1l*bar[index_high])<<32);
	printf("reg_latency: %ld\n",latency);
	printf("average: %f us \n",1.0*latency*(3.3/1000)/num);
	printf("\n");
}