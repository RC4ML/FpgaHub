#ifndef __FPGA_CONTROL_HPP__
#define __FPGA_CONTROL_HPP__
#include <stdio.h>
#include <string>
#include <unistd.h>
using namespace std;

void qp_init(volatile uint32_t *bar, uint32_t credit);
void reset(volatile uint32_t *bar);
void print_latency(volatile uint32_t *bar,int index_high,int index_low,string str, int num);
#endif