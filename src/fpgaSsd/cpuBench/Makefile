SPDK_DIR = /home/ff/hyperion/spdk

.PHONY:all
.PHONY:run

exe:
	g++ --std=c++17 -mcmodel=medium cpu_bench.cpp.cpp -o cpu_bench.cpp -lcudart -L/usr/local/cuda/lib64 -lqdma \
	-g -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -fno-strict-aliasing \
	-I$(SPDK_DIR)/include -march=native -Wformat -Wformat-security -D_GNU_SOURCE -fPIC -fstack-protector -fno-common \
	-I$(SPDK_DIR)/isa-l/.. -I$(SPDK_DIR)/isalbuild -I$(SPDK_DIR)/isa-l-crypto/.. \
	-I$(SPDK_DIR)/isalcryptobuild -DNDEBUG -O2 -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=2 -DSPDK_GIT_COMMIT=6ee9cd257 -pthread -std=c++17 \
	-Wl,-z,relro,-z,now -Wl,-z,noexecstack -fuse-ld=bfd -Wl,-rpath=$(SPDK_DIR)/isa-l/.libs -Wl,-rpath=$(SPDK_DIR)/isa-l-crypto/.libs \
	-L$(SPDK_DIR)/build/lib -Wl,--whole-archive -Wl,--no-as-needed -lspdk_sock_posix -lspdk_nvme -lspdk_keyring -lspdk_sock -lspdk_trace -lspdk_rpc -lspdk_jsonrpc -lspdk_json -lspdk_dma -lspdk_vmd -lspdk_util -lspdk_log \
	-Wl,--no-whole-archive $(SPDK_DIR)/build/lib/libspdk_env_dpdk.so -Wl,--no-as-needed $(SPDK_DIR)/dpdk/build/lib/librte_bus_pci.so $(SPDK_DIR)/dpdk/build/lib/librte_cryptodev.so \
	$(SPDK_DIR)/dpdk/build/lib/librte_dmadev.so $(SPDK_DIR)/dpdk/build/lib/librte_eal.so $(SPDK_DIR)/dpdk/build/lib/librte_ethdev.so \
	$(SPDK_DIR)/dpdk/build/lib/librte_hash.so $(SPDK_DIR)/dpdk/build/lib/librte_kvargs.so $(SPDK_DIR)/dpdk/build/lib/librte_log.so $(SPDK_DIR)/dpdk/build/lib/librte_mbuf.so \
	$(SPDK_DIR)/dpdk/build/lib/librte_mempool.so $(SPDK_DIR)/dpdk/build/lib/librte_mempool_ring.so $(SPDK_DIR)/dpdk/build/lib/librte_net.so $(SPDK_DIR)/dpdk/build/lib/librte_pci.so \
	$(SPDK_DIR)/dpdk/build/lib/librte_power.so $(SPDK_DIR)/dpdk/build/lib/librte_rcu.so $(SPDK_DIR)/dpdk/build/lib/librte_ring.so $(SPDK_DIR)/dpdk/build/lib/librte_telemetry.so \
	$(SPDK_DIR)/dpdk/build/lib/librte_vhost.so -Wl,-as-needed -Wl,-rpath=$(SPDK_DIR)/dpdk/build/lib  -L$(SPDK_DIR)/isa-l/.libs -lisal -L$(SPDK_DIR)/isa-l-crypto/.libs -lisal_crypto -pthread -lrt -luuid -lssl -lcrypto -lm -lfuse3 -lkeyutils -laio

	

run:
	HUGENODE='nodes_hp[0]=8192,nodes_hp[1]=8192' PCI_BLOCKED="0000:68:00.0" $(SPDK_DIR)/scripts/setup.sh
	LD_LIBRARY_PATH=$(SPDK_DIR)/isa-l-crypto/.libs:$(SPDK_DIR)/isa-l/.libs:$(SPDK_DIR)/build/lib/:$(SPDK_DIR)/dpdk/build/lib:$(LD_LIBRARY_PATH) ./cpu_bench.cpp
	$(SPDK_DIR)/scripts/setup.sh reset