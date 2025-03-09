#ifndef _COUNTER_P4_
#define _COUNTER_P4_


#include "types.p4"
#include "config.p4"

struct counter_register_value
{
    bit<CNT_WIDTH> slot_0;
    bit<CNT_WIDTH> slot_1;
}

control RegisterCounter(
    in    bit<1> slot,
    in    bit<IDX_WIDTH> index,
    in    bit<MAP_WIDTH> status,
    inout bit<CNT_WIDTH> counter_value)
{
    Register<counter_register_value, bit<IDX_WIDTH>>(SLOT_SIZE) counter_reg;

    RegisterAction<
        counter_register_value,
        bit<IDX_WIDTH>,
        bit<CNT_WIDTH>
    >(counter_reg) read_0 = {
        void apply(inout counter_register_value register_data, out bit<CNT_WIDTH> result)
        {
            result = register_data.slot_0;
        }
    };

    RegisterAction<
        counter_register_value,
        bit<IDX_WIDTH>,
        bit<CNT_WIDTH>
    >(counter_reg) read_1 = {
        void apply(inout counter_register_value register_data, out bit<CNT_WIDTH> result)
        {
            result = register_data.slot_1;
        }
    };

    RegisterAction<
        counter_register_value,
        bit<IDX_WIDTH>,
        bit<CNT_WIDTH>
    >(counter_reg) increment_0 = {
        void apply(inout counter_register_value register_data, out bit<CNT_WIDTH> result)
        {
            register_data.slot_0 = register_data.slot_0 + 1;
            result = register_data.slot_0;
        }
    };

    RegisterAction<
        counter_register_value,
        bit<IDX_WIDTH>,
        bit<CNT_WIDTH>
    >(counter_reg) increment_1 = {
        void apply(inout counter_register_value register_data, out bit<CNT_WIDTH> result)
        {
            register_data.slot_1 = register_data.slot_1 + 1;
            result = register_data.slot_1;
        }
    };

    action read_counter_0()
    {
        counter_value = read_0.execute(index);
    }

    action read_counter_1()
    {
        counter_value = read_1.execute(index);
    }

    action increment_counter_0()
    {
        counter_value = increment_0.execute(index);
    }

    action increment_counter_1()
    {
        counter_value = increment_1.execute(index);
    }

    table count
    {
        key = {
            status : ternary;
            slot : ternary;
        }

        actions = {
            read_counter_0;
            read_counter_1;
            increment_counter_0;
            increment_counter_1;
            @defaultonly NoAction;
        }

        size = 4;
        const entries = {
            (0, 0) : increment_counter_0();
            (0, 1) : increment_counter_1();
            (_, 0) : read_counter_0();
            (_, 1) : read_counter_1();
        }

        default_action = NoAction;
    }

    apply
    {
        count.apply();
    }
}


#endif //_COUNTER_P4_
