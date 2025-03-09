#ifndef _BITMAP_P4_
#define _BITMAP_P4_


#include "config.p4"
#include "types.p4"

control CheckBitmap(
    in    bit<1> slot,
    in    bit<IDX_WIDTH> index,
    in    PortId_t ingress_port,
    inout bit<MAP_WIDTH> bitmap,
    inout bit<MAP_WIDTH> status)
{
    // bitmap mapping
    action get_bitmap(bit<MAP_WIDTH> map)
    {
        bitmap = map;
    }

    table bitmap_mapping
    {
        key = {
            bitmap: exact;
            ingress_port: exact;
        }

        actions = {
            get_bitmap;
            NoAction;
        }

        size = 32;
        const entries = {
            (0,   0) : get_bitmap(1 <<  0);
            (0,   4) : get_bitmap(1 <<  1);
            (0,   8) : get_bitmap(1 <<  2);
            (0,  12) : get_bitmap(1 <<  3);
            (0,  16) : get_bitmap(1 <<  4);
            (0,  20) : get_bitmap(1 <<  5);
            (0,  24) : get_bitmap(1 <<  6);
            (0,  28) : get_bitmap(1 <<  7);
            (0,  32) : get_bitmap(1 <<  8);
            (0,  36) : get_bitmap(1 <<  9);
            (0,  40) : get_bitmap(1 << 10);
            (0,  44) : get_bitmap(1 << 11);
            (0,  48) : get_bitmap(1 << 12);
            (0,  52) : get_bitmap(1 << 13);
            (0,  56) : get_bitmap(1 << 14);
            (0,  60) : get_bitmap(1 << 15);
            (0, 128) : get_bitmap(1 << 16);
            (0, 132) : get_bitmap(1 << 17);
            (0, 136) : get_bitmap(1 << 18);
            (0, 140) : get_bitmap(1 << 19);
            (0, 144) : get_bitmap(1 << 20);
            (0, 148) : get_bitmap(1 << 21);
            (0, 152) : get_bitmap(1 << 22);
            (0, 156) : get_bitmap(1 << 23);
            (0, 160) : get_bitmap(1 << 24);
            (0, 164) : get_bitmap(1 << 25);
            (0, 168) : get_bitmap(1 << 26);
            (0, 172) : get_bitmap(1 << 27);
            (0, 176) : get_bitmap(1 << 28);
            (0, 180) : get_bitmap(1 << 29);
            (0, 184) : get_bitmap(1 << 30);
            (0, 188) : get_bitmap(1 << 31);
        }

        const default_action = NoAction;
    }

    Register<bit<MAP_WIDTH>, bit<IDX_WIDTH>>(SLOT_SIZE) bitmap_reg_0;
    Register<bit<MAP_WIDTH>, bit<IDX_WIDTH>>(SLOT_SIZE) bitmap_reg_1;

    RegisterAction<
        bit<MAP_WIDTH>,
        bit<IDX_WIDTH>,
        bit<MAP_WIDTH>
    >(bitmap_reg_0) merge_0 = {
        void apply(inout bit<MAP_WIDTH> register_data, out bit<MAP_WIDTH> result)
        {
            bit<MAP_WIDTH> temp = register_data & bitmap;
            register_data = register_data | bitmap;
            result = temp;
        }
    };
    RegisterAction<
        bit<MAP_WIDTH>,
        bit<IDX_WIDTH>,
        bit<MAP_WIDTH>
    >(bitmap_reg_0) clear_0 = {
        void apply(inout bit<MAP_WIDTH> register_data)
        {
            register_data = register_data & ~(bitmap);
        }
    };

    RegisterAction<
        bit<MAP_WIDTH>,
        bit<IDX_WIDTH>,
        bit<MAP_WIDTH>
    >(bitmap_reg_1) merge_1 = {
        void apply(inout bit<MAP_WIDTH> register_data, out bit<MAP_WIDTH> result)
        {
            bit<MAP_WIDTH> temp = register_data & bitmap;
            register_data = register_data | bitmap;
            result = temp;
        }
    };
    RegisterAction<
        bit<MAP_WIDTH>,
        bit<IDX_WIDTH>,
        bit<MAP_WIDTH>
    >(bitmap_reg_1) clear_1 = {
        void apply(inout bit<MAP_WIDTH> register_data)
        {
            register_data = register_data & ~(bitmap);
        }
    };

    action bitmap_merge_0()
    {
        status = merge_0.execute(index);
    }

    action bitmap_clear_0()
    {
        clear_0.execute(index);
    }

    action bitmap_merge_1()
    {
        status = merge_1.execute(index);
    }

    action bitmap_clear_1()
    {
        clear_1.execute(index);
    }

    apply
    {
        bitmap_mapping.apply();

        if(slot == 0)
        {
            bitmap_merge_0();
            bitmap_clear_1();
        }
        else
        {
            bitmap_merge_1();
            bitmap_clear_0();
        }
    }
}


#endif //_BITMAP_P4_
