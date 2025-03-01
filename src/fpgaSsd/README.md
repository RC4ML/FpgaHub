# FPGA-SSD Example.

This folder provides:
* An example bitstream and of our FPGA-SSD design benchmarking *a single* SSD via Alveo U50 FPGA board;
* Code to benchmark CPU core usage manipulating multiple SSDs (to reproduce Figure 9).

## To Run FPGA and Corresponding Software:
1. Install an SSD into your server. 
2. Prepare an Alveo U50 FPGA, plug it into PCIe slot of your server.
3. Program the bitstream and restart your server.
4. Install the QDMA driver and library following the guide in [[https://github.com/RC4ML/rc4ml_qdma]].
5. Edit line 27 of `fpgasw/LatencyBenchmark.cpp`. You should change the first entry of `pci_id` array to the PCI bus ID of your SSDs. You can use following Linux command to query the bus ID:  
`lspci | grep "Non-Volatile memory controller"`
6. Compile the software code and enjoy your benchmarking.

## To Run CPU Benchmark:
1. Install [SPDK](https://github.com/spdk/spdk).
2. Change Line 1 of `cpuBench/Makefile` to your installed SPDK path.
3. Compile via `make exe`
4. Run via `make run`