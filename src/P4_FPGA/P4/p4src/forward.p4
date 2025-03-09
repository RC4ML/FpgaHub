#ifndef _FORWARD_P4_
#define _FORWARD_P4_


#include "types.p4"
#include "config.p4"

// #define DEBUG

control Forward(
    in    bit<MAP_WIDTH> status,
    in    bit<CNT_WIDTH> counter_value,
    // Intrinsic
    in    ingress_intrinsic_metadata_t              ig_intr_md,
    in    ingress_intrinsic_metadata_from_parser_t  ig_prsr_md,
    inout ingress_intrinsic_metadata_for_deparser_t ig_dprsr_md,
    inout ingress_intrinsic_metadata_for_tm_t       ig_tm_md)
{
    action multicast(MulticastGroupId_t mcast_grp)
    {
        ig_tm_md.mcast_grp_a = mcast_grp;
    }

    action reflect()
    {
        ig_tm_md.ucast_egress_port = ig_intr_md.ingress_port;
    }

    action send(PortId_t port)
	{
        #ifdef DEBUG
	    ig_tm_md.ucast_egress_port = port;
        #endif
	}

    table forward
    {
        key = {
            counter_value : ternary;
            status : ternary;
        }

        actions = {
            multicast;
            reflect;
            send;
            NoAction;
        }
        
        size = 4;
        const entries = {
            /* complete */
            /* new pkt - reset & multicast */
            (0 &&& NODE_SIZE - 1, 0) : multicast(1);
            /* resend - shadow_copy & unicast */
            (0 &&& NODE_SIZE - 1, _) : reflect();
            /* incomplete */
            /* new pkt - reduce */
            (                  _, 0) : send(192);
            /* resend - do nothing */
            (                  _, _) : send(192);
        }
        
        const default_action = NoAction;
    }

    apply
    {
        forward.apply();
    }
}


#endif //_FORWARD_P4_
