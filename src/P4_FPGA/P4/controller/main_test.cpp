//
// Created by Alex_Li on 2021/8/23.
//

#include <iostream>
#include <ctime>
#include <pthread.h>

#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/filter.h>

#include <bf_rt/bf_rt_session.hpp>
#include <bf_rt/bf_rt_common.h>
#include <bf_rt/bf_rt_info.hpp>
#include <bf_rt/bf_rt_init.hpp>
#include <bf_rt/bf_rt_table_key.hpp>
#include <bf_rt/bf_rt_table_data.hpp>
#include <bf_rt/bf_rt_table.hpp>
#include <pkt_mgr/bf_pkt.h>
#include <pkt_mgr/pkt_mgr_intf.h>

extern "C"
{
#include <bf_switchd/bf_switchd.h>
}

#include "my_header.h"

using namespace std;

int main()
{
	// init bf_switchd
	bf_switchd_context_t *switchd_context;
	if((switchd_context = (bf_switchd_context_t *) calloc(1, sizeof(bf_switchd_context_t))) == nullptr)
	{
		printf("Cannot Allocate switchd context\n");
		exit(1);
	}
	switchd_context->install_dir = strdup(string("/root/Software/bf-sde/install").c_str());
	switchd_context->conf_file = strdup(
			string("/root/Software/bf-sde/install/share/p4/targets/tofino/KVS.conf").c_str());
	switchd_context->running_in_background = true;
	switchd_context->dev_sts_thread = true;
	switchd_context->dev_sts_port = 7777;
	switchd_context->kernel_pkt = true;

	bf_status_t status = bf_switchd_lib_init(switchd_context);
	if(status != BF_SUCCESS)
	{
		cout << "ERROR: Cannot init switchd: " << bf_err_str(status) << endl;
	}

	// open session
	std::shared_ptr<bfrt::BfRtSession> session = bfrt::BfRtSession::sessionCreate();
	if(session == nullptr)
	{
		return 1;
	}

	// connect to device
	bf_rt_target_t dev_tgt;
	dev_tgt.dev_id = 0;
	dev_tgt.pipe_id = 0xffff; // All pipes

	const bfrt::BfRtInfo *bfrtInfo = nullptr;
	auto &devMgr = bfrt::BfRtDevMgr::getInstance();
	if((status = devMgr.bfRtInfoGet(dev_tgt.dev_id, "KVS", &bfrtInfo)) != BF_SUCCESS)
	{
		cout << "ERROR: Cannot get BfRt info: " << bf_err_str(status) << endl;
		return status;
	}

	cout << "-----------------------------------\n"
	     << "successfully connected to device" << endl;

//	// init port
//	bf_pal_front_port_handle_t port_hdl;
//	port_hdl.conn_id = 33;
//	port_hdl.chnl_id = 0;
//
//	if((status = bf_port_add(dev_tgt.dev_id, 64, BF_SPEED_10G, BF_FEC_TYP_NONE)) != BF_SUCCESS)
//	{
//		cout << "ERROR: Cannot add port enp4s0f1: " << bf_err_str(status) << endl;
//		return status;
//	}
//	if((status = bf_pm_port_direction_set(dev_tgt.dev_id, &port_hdl, PM_PORT_DIR_DEFAULT)) != BF_SUCCESS)
//	{
//		cout << "ERROR: Cannot set port direction: " << bf_err_str(status) << endl;
//		return status;
//	}
//	if((status = bf_port_add(dev_tgt.dev_id, 66, BF_SPEED_10G, BF_FEC_TYP_NONE)) != BF_SUCCESS)
//	{
//		cout << "ERROR: Cannot add port enp4s0f0: " << bf_err_str(status) << endl;
//		return status;
//	}
//
//	if((status = bf_pm_pltfm_front_port_ready_for_bringup(dev_tgt.dev_id, &port_hdl, true)) != BF_SUCCESS)
//	{
//		cout << "ERROR: Cannot enable port enp4s0f1: " << bf_err_str(status) << endl;
//		return status;
//	}
//
//	if((status = bf_port_enable(dev_tgt.dev_id, 64, true)) != BF_SUCCESS)
//	{
//		cout << "ERROR: Cannot enable port enp4s0f1: " << bf_err_str(status) << endl;
//		return status;
//	}
//	if((status = bf_port_enable(dev_tgt.dev_id, 66, true)) != BF_SUCCESS)
//	{
//		cout << "ERROR: Cannot enable port enp4s0f0: " << bf_err_str(status) << endl;
//		return status;
//	}
//
//	if((status = bf_pm_pltfm_front_port_eligible_for_autoneg(dev_tgt.dev_id, &port_hdl, true)) != BF_SUCCESS)
//	{
//		cout << "ERROR: Cannot enable auto negotiate: " << bf_err_str(status) << endl;
//		return status;
//	}
//
//	cout << "successfully add ports" << endl;

	// init network
//	int sock_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
//	if(-1 == sock_fd)
//	{
//		cout << "Create socket error(" << errno << "): " << strerror(errno) << endl;
//		return -1;
//	}
//	else
//		cout << "Create socket success" << endl;
//
//	ifreq ifr{};
//	bzero(&ifr, sizeof(ifr));
//	strncpy(ifr.ifr_name, "enp4s0f1", sizeof(ifr.ifr_name));
//	ioctl(sock_fd, SIOCGIFINDEX, &ifr);
//
//	sockaddr_ll sl{};
//	bzero(&sl, sizeof(sl));
//	sl.sll_family = PF_PACKET;
//	sl.sll_protocol = htons(ETH_P_ALL);
//	sl.sll_ifindex = ifr.ifr_ifindex;
//
//	if(-1 == bind(sock_fd, (sockaddr *) &sl, sizeof(sl)))
//	{
//		cout << "Bind error(" << errno << "): " << strerror(errno) << endl;
//		return -1;
//	}
//	else
//		cout << "Bind success" << endl;
//
//	sock_fprog filter{};
//	sock_filter code[] = { // tcpdump -dd port 8888 -s 0
//			{0x28, 0,  0,  0x0000000c},
//			{0x15, 0,  8,  0x000086dd},
//			{0x30, 0,  0,  0x00000014},
//			{0x15, 2,  0,  0x00000084},
//			{0x15, 1,  0,  0x00000006},
//			{0x15, 0,  17, 0x00000011},
//			{0x28, 0,  0,  0x00000036},
//			{0x15, 14, 0,  0x000022b8},
//			{0x28, 0,  0,  0x00000038},
//			{0x15, 12, 13, 0x000022b8},
//			{0x15, 0,  12, 0x00000800},
//			{0x30, 0,  0,  0x00000017},
//			{0x15, 2,  0,  0x00000084},
//			{0x15, 1,  0,  0x00000006},
//			{0x15, 0,  8,  0x00000011},
//			{0x28, 0,  0,  0x00000014},
//			{0x45, 6,  0,  0x00001fff},
//			{0xb1, 0,  0,  0x0000000e},
//			{0x48, 0,  0,  0x0000000e},
//			{0x15, 2,  0,  0x000022b8},
//			{0x48, 0,  0,  0x00000010},
//			{0x15, 0,  1,  0x000022b8},
//			{0x6,  0,  0,  0x00040000},
//			{0x6,  0,  0,  0x00000000},
//	};
//
//	filter.len = sizeof(code) / sizeof(sock_filter);
//	filter.filter = code;
//
//	if(-1 == setsockopt(sock_fd, SOL_SOCKET, SO_ATTACH_FILTER, &filter, sizeof(filter)))
//	{
//		cout << "Set sockopt error(" << errno << "): " << strerror(errno) << endl;
//		return -1;
//	}
//	else
//		cout << "Set sockopt success" << endl;

	// test
	bf_pkt_init();
	bf_pkt pkt;

	// set table in Ingress
	const bfrt::BfRtTable *key_value_table = nullptr;
	if((status = bfrtInfo->bfrtTableFromNameGet("Ingress.key_value", &key_value_table)) != BF_SUCCESS)
	{
		cout << "ERROR: Cannot get table: " << bf_err_str(status) << endl;
		return status;
	}

	// set action in Ingress
	bf_rt_id_t kv_read_action_id = 0;
	if((status = key_value_table->actionIdGet("Ingress.kv_read", &kv_read_action_id)) != BF_SUCCESS)
	{
		cout << "ERROR: Cannot get action: " << bf_err_str(status) << endl;
		return status;
	}

	// set table key
	bf_rt_id_t kv_key_field_id = 0;
	if((status = key_value_table->keyFieldIdGet("hdr.kv.key", &kv_key_field_id)) != BF_SUCCESS)
	{
		cout << "ERROR: Cannot get key field: " << bf_err_str(status) << endl;
		return status;
	}

	// set table data
	bf_rt_id_t kv_read_action_value_data_field_id = 0;
	if((status = key_value_table->dataFieldIdGet("value", kv_read_action_id, &kv_read_action_value_data_field_id)) !=
	   BF_SUCCESS)
	{
		cout << "ERROR: Cannot get data field: " << bf_err_str(status) << endl;
		return status;
	}

	unique_ptr<bfrt::BfRtTableKey> tableKey;
	unique_ptr<bfrt::BfRtTableData> tableData;

	if((status = key_value_table->keyAllocate(&tableKey)) != BF_SUCCESS)
	{
		cout << "ERROR: Cannot allocate key: " << bf_err_str(status) << endl;
		return status;
	}
	if((status = key_value_table->dataAllocate(&tableData)) != BF_SUCCESS)
	{
		cout << "ERROR: Cannot allocate data: " << bf_err_str(status) << endl;
		return status;
	}

	// pcap callback action
//	u_char pkt_data[65536] = {};

//	string Proto[] = {"Reserved", "ICMP", "IGMP", "GGP", "IP", "ST", "TCP"};
//	while(true)
//	{
//		auto recv_len = recvfrom(sock_fd, pkt_data, sizeof(pkt_data), 0, nullptr, nullptr);
//		if(-1 == recv_len)
//		{
//			cout << "Recv error(" << errno << "): " << strerror(errno) << endl;
//			return -1;
//		}
//		else
//		{
//			auto eth_header = (ETH_HEADER *) pkt_data;
//			printf("---------------Begin Analysis-----------------\n");
//			printf("----------------------------------------------\n");
//			printf("Packet length: %ld \n", recv_len);
//
//			if(recv_len >= 14)
//			{
//				auto ip_header = (IP4_HEADER *) (pkt_data + 14);
//
//				string strType;
//				if(ip_header->proto > 7)
//					strType = "IP/UNKNWN";
//				else
//					strType = Proto[ip_header->proto];
//
//				printf("Source MAC : %02X-%02X-%02X-%02X-%02X-%02X    ==>    ",
//				       eth_header->SrcMac[0],
//				       eth_header->SrcMac[1],
//				       eth_header->SrcMac[2],
//				       eth_header->SrcMac[3],
//				       eth_header->SrcMac[4],
//				       eth_header->SrcMac[5]);
//				printf("Dest MAC : %02X-%02X-%02X-%02X-%02X-%02X\n",
//				       eth_header->DestMac[0],
//				       eth_header->DestMac[1],
//				       eth_header->DestMac[2],
//				       eth_header->DestMac[3],
//				       eth_header->DestMac[4],
//				       eth_header->DestMac[5]);
//
//				printf("Source IP  : %d.%d.%d.%d    ==>    ",
//				       ip_header->sourceIP[0],
//				       ip_header->sourceIP[1],
//				       ip_header->sourceIP[2],
//				       ip_header->sourceIP[3]);
//				printf("Dest IP  : %d.%d.%d.%d\n",
//				       ip_header->destIP[0],
//				       ip_header->destIP[1],
//				       ip_header->destIP[2],
//				       ip_header->destIP[3]);
//
//				cout << "Protocol : " << strType << endl;
//
//				for(int i = 0; i < (int) recv_len; ++i)
//				{
//					printf(" %02x", pkt_data[i]);
//					if((i + 1) % 16 == 0)
//						printf("\n");
//				}
//				printf("\n\n");
//			}
//		}
//	}

//	set<unsigned int> keySet;
//	session->beginBatch();
//	while(true)
//	{
//		auto len = recvfrom(sock_fd, pkt_data, sizeof(pkt_data), 0, nullptr, nullptr);
//		if(-1 == len)
//		{
//			cout << "Receive error(" << errno << "): " << strerror(errno) << endl;
//			return -1;
//		}
//
//		auto *nc_header = (NET_HEADER *) (pkt_data + 42);
//		uint64_t key = ntohl(nc_header->key);
//		uint64_t value = ntohl(nc_header->value);
//
//		if(nc_header->op == 1)
//		{
//			key_value_table->keyReset(tableKey.get());
//			key_value_table->dataReset(kv_read_action_id, tableData.get());
//
//			if((status = tableKey->setValue(kv_key_field_id, key)) != BF_SUCCESS)
//			{
//				cout << "ERROR: Cannot set key value: " << bf_err_str(status) << endl;
//				return status;
//			}
//			if((status = tableData->setValue(kv_read_action_value_data_field_id, value)) != BF_SUCCESS)
//			{
//				cout << "ERROR: Cannot set data value: " << bf_err_str(status) << endl;
//				return status;
//			}
//
//			if(keySet.find(key) != keySet.cend())
//			{
//				if((status = key_value_table->tableEntryMod(*session, dev_tgt, *tableKey, *tableData)) != BF_SUCCESS)
//				{
//					cout << "ERROR: Cannot modify entry: " << bf_err_str(status) << endl;
//				}
//			}
//			else
//			{
//				if((status = key_value_table->tableEntryAdd(*session, dev_tgt, *tableKey, *tableData)) != BF_SUCCESS)
//				{
//					cout << "ERROR: Cannot add entry: " << bf_err_str(status) << endl;
//				}
//				else
//				{
//					keySet.insert(key);
//				}
//			}
//			session->flushBatch();
//			session->sessionCompleteOperations();
//
//			cout << "Successfully add entry: {" << key << ", " << value << "}"
//			     << endl;
//		}
//	}
//	session->endBatch(true);

	// destroy session
	if((status = session->sessionDestroy()) != BF_SUCCESS)
	{
		cout << "ERROR: Cannot destroy session: " << bf_err_str(status) << endl;
		return 1;
	}

	// clean
	pthread_join(switchd_context->tmr_t_id, nullptr);
	pthread_join(switchd_context->dma_t_id, nullptr);
	pthread_join(switchd_context->int_t_id, nullptr);
	pthread_join(switchd_context->pkt_t_id, nullptr);
	pthread_join(switchd_context->port_fsm_t_id, nullptr);
	pthread_join(switchd_context->drusim_t_id, nullptr);
	pthread_join(switchd_context->accton_diag_t_id, nullptr);

	free(switchd_context);

	return 0;
}
