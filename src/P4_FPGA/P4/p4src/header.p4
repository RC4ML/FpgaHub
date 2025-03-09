#ifndef _HEADER_P4_
#define _HEADER_P4_


#include "types.p4"
#include "config.p4"

/***********************  H E A D E R S  ************************/

header ethernet_h
{
    bit<48> dst_addr;
    bit<48> src_addr;
    bit<16> ether_type;
}

header arp_h
{
    bit<16> hw_type;
    bit<16> proto_type;
    bit<8>  hw_addr_len;
    bit<8>  proto_addr_len;
    bit<16> opcode;

    bit<48> src_hw_addr;
    bit<32> src_proto_addr;
    bit<48> dst_hw_addr;
    bit<32> dst_proto_addr;
}

header ipv4_h
{
    bit<4>  version;
    bit<4>  ihl;
    bit<8>  diffserv;
    bit<16> total_len;
    bit<16> identification;
    bit<3>  flags;
    bit<13> frag_offset;
    bit<8>  ttl;
    bit<8>  protocol;
    bit<16> hdr_checksum;
    bit<32> src_addr;
    bit<32> dst_addr;
}

header tcp_h
{
    bit<16> src_port;
    bit<16> dst_port;
    bit<32> seq_no;
    bit<32> ack_no;
    bit<4>  data_offset;
    bit<3>  res;
    bit<3>  ecn;
    bit<6>  ctrl;
    bit<16> window;
    bit<16> checksum;
    bit<16> urgent_ptr;
}

header udp_t
{
    bit<16> src_port;
    bit<16> dst_port;
    bit<16> len;
    bit<16> checksum;
}

/* Customized struct */

header reduce_header_t
{
    @padding bit<IDX_PADDING_WIDTH> __pad0;

    bit<1>         slot;
    bit<IDX_WIDTH> index;

    bit<32>        seq_no;
    bit<MAP_WIDTH> bitmap;

    @padding bit<HDR_PADDING_WIDTH> __pad1;
}

#define VALUE(i, j) \
    bit<VAL_WIDTH> value_##i##_##j

#define VALUE_STAGE(i) \
    VALUE(i, 0);       \
    VALUE(i, 1);       \
    VALUE(i, 2);       \
    VALUE(i, 3)

header reduce_data_t
{
    VALUE_STAGE(0);
    VALUE_STAGE(1);
    VALUE_STAGE(2);
    VALUE_STAGE(3);
    // VALUE_STAGE(4);
    // VALUE_STAGE(5);
    // VALUE_STAGE(6);
    // VALUE_STAGE(7);
}

struct my_ingress_headers_t
{
    ethernet_h ethernet;
    arp_h      arp;
    ipv4_h     ipv4;
    tcp_h      tcp;
    udp_t      udp;

    reduce_header_t rh;
    reduce_data_t   rd;
}

/******  G L O B A L   I N G R E S S   M E T A D A T A  *********/

struct my_ingress_metadata_t
{
    bit<32> dst_ipv4;

    bit<MAP_WIDTH> status;
    bit<CNT_WIDTH> counter_value;
}

/***********************  H E A D E R S  ************************/

struct my_egress_headers_t
{
}

/********  G L O B A L   E G R E S S   M E T A D A T A  *********/

struct my_egress_metadata_t
{
}


#endif //_HEADER_P4_
