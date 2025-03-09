#ifndef _TYPES_P4_
#define _TYPES_P4_


const bit<16> ETHERTYPE_IPV4 = 0x0800;
const bit<16> ETHERTYPE_ARP  = 0x0806;

const bit<8> IPV4_PROTOCOL_TCP = 6;
const bit<8> IPV4_PROTOCOL_UDP = 17;

const int IPV4_HOST_SIZE = 65536;
const int IPV4_LPM_SIZE  = 12288;


#endif //_TYPES_P4_