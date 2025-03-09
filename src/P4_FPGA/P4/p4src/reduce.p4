#ifndef _REDUCE_P4_
#define _REDUCE_P4_


#include "types.p4"
#include "config.p4"
#include "header.p4"
#include "bitmap.p4"
#include "counter.p4"

struct register_value_t
{
    bit<VAL_WIDTH> slot_0;
    bit<VAL_WIDTH> slot_1;
}

#define REDUCE_REG(i, j)                                                            \
    Register<register_value_t, bit<IDX_WIDTH>>(SLOT_SIZE) reduce_reg_##i##_##j;     \
    RegisterAction<                                                                 \
        register_value_t,                                                           \
        bit<IDX_WIDTH>,                                                             \
        bit<VAL_WIDTH>                                                              \
    >(reduce_reg_##i##_##j) reduce_##i##_##j##_slot_0 = {                           \
        void apply(inout register_value_t register_data, out bit<VAL_WIDTH> result) \
        {                                                                           \
            register_data.slot_0 = register_data.slot_0 + data.value_##i##_##j;     \
            result = register_data.slot_0;                                          \
        }                                                                           \
    };                                                                              \
    RegisterAction<                                                                 \
        register_value_t,                                                           \
        bit<IDX_WIDTH>,                                                             \
        bit<VAL_WIDTH>                                                              \
    >(reduce_reg_##i##_##j) reduce_##i##_##j##_slot_1 = {                           \
        void apply(inout register_value_t register_data, out bit<VAL_WIDTH> result) \
        {                                                                           \
            register_data.slot_1 = register_data.slot_1 + data.value_##i##_##j;     \
            result = register_data.slot_1;                                          \
        }                                                                           \
    };                                                                              \
    RegisterAction<                                                                 \
        register_value_t,                                                           \
        bit<IDX_WIDTH>,                                                             \
        bit<VAL_WIDTH>                                                              \
    >(reduce_reg_##i##_##j) reduce_reset_##i##_##j##_slot_0 = {                     \
        void apply(inout register_value_t register_data, out bit<VAL_WIDTH> result) \
        {                                                                           \
            register_data.slot_0 = register_data.slot_0 + data.value_##i##_##j;     \
            result = register_data.slot_0;                                          \
            register_data.slot_1 = 0;                                               \
        }                                                                           \
    };                                                                              \
    RegisterAction<                                                                 \
        register_value_t,                                                           \
        bit<IDX_WIDTH>,                                                             \
        bit<VAL_WIDTH>                                                              \
    >(reduce_reg_##i##_##j) reduce_reset_##i##_##j##_slot_1 = {                     \
        void apply(inout register_value_t register_data, out bit<VAL_WIDTH> result) \
        {                                                                           \
            register_data.slot_1 = register_data.slot_1 + data.value_##i##_##j;     \
            result = register_data.slot_1;                                          \
            register_data.slot_0 = 0;                                               \
        }                                                                           \
    };                                                                              \
    RegisterAction<                                                                 \
        register_value_t,                                                           \
        bit<IDX_WIDTH>,                                                             \
        bit<VAL_WIDTH>                                                              \
    >(reduce_reg_##i##_##j) get_shadow_copy_##i##_##j##_slot_0 = {                  \
        void apply(inout register_value_t register_data, out bit<VAL_WIDTH> result) \
        {                                                                           \
            result = register_data.slot_1;                                          \
        }                                                                           \
    };                                                                              \
    RegisterAction<                                                                 \
        register_value_t,                                                           \
        bit<IDX_WIDTH>,                                                             \
        bit<VAL_WIDTH>                                                              \
    >(reduce_reg_##i##_##j) get_shadow_copy_##i##_##j##_slot_1 = {                  \
        void apply(inout register_value_t register_data, out bit<VAL_WIDTH> result) \
        {                                                                           \
            result = register_data.slot_0;                                          \
        }                                                                           \
    }

#define REDUCE_REG_STAGE(i) \
    REDUCE_REG(i, 0);       \
    REDUCE_REG(i, 1);       \
    REDUCE_REG(i, 2);       \
    REDUCE_REG(i, 3)

#define REDUCE_ACTION(i, j)                                                       \
    action reduce_##i##_##j##_slot_0_act()                                        \
    {                                                                             \
        data.value_##i##_##j = reduce_##i##_##j##_slot_0.execute(index);          \
    }                                                                             \
    action reduce_##i##_##j##_slot_1_act()                                        \
    {                                                                             \
        data.value_##i##_##j = reduce_##i##_##j##_slot_1.execute(index);          \
    }                                                                             \
    action reduce_reset_##i##_##j##_slot_0_act()                                  \
    {                                                                             \
        data.value_##i##_##j = reduce_reset_##i##_##j##_slot_0.execute(index);    \
    }                                                                             \
    action reduce_reset_##i##_##j##_slot_1_act()                                  \
    {                                                                             \
        data.value_##i##_##j = reduce_reset_##i##_##j##_slot_1.execute(index);    \
    }                                                                             \
    action get_shadow_copy_##i##_##j##_slot_0_act()                               \
    {                                                                             \
        /* data.value_##i##_##j = get_shadow_copy_##i##_##j##_slot_0.execute(index); */ \
    }                                                                             \
    action get_shadow_copy_##i##_##j##_slot_1_act()                               \
    {                                                                             \
        /* data.value_##i##_##j = get_shadow_copy_##i##_##j##_slot_1.execute(index); */ \
    }

#define REDUCE_ACTION_STAGE(i) \
    REDUCE_ACTION(i, 0)        \
    REDUCE_ACTION(i, 1)        \
    REDUCE_ACTION(i, 2)        \
    REDUCE_ACTION(i, 3)

#define REDUCE_TABLE(i, j)                                                          \
    table reduce_table_##i##_##j                                                    \
    {                                                                               \
        key = {                                                                     \
            meta.counter_value : ternary;                                           \
            meta.status : ternary;                                                  \
            slot : ternary;                                                         \
        }                                                                           \
                                                                                    \
        actions = {                                                                 \
            reduce_##i##_##j##_slot_0_act;                                          \
            reduce_##i##_##j##_slot_1_act;                                          \
            reduce_reset_##i##_##j##_slot_0_act;                                    \
            reduce_reset_##i##_##j##_slot_1_act;                                    \
            get_shadow_copy_##i##_##j##_slot_0_act;                                 \
            get_shadow_copy_##i##_##j##_slot_1_act;                                 \
            NoAction;                                                               \
        }                                                                           \
                                                                                    \
        size = 7;                                                                   \
        const entries = {                                                           \
            /* complete */                                                          \
            /* new pkt - reset & multicast */                                       \
            (0 &&& NODE_SIZE - 1, 0, 0) : reduce_reset_##i##_##j##_slot_0_act();    \
            (0 &&& NODE_SIZE - 1, 0, 1) : reduce_reset_##i##_##j##_slot_0_act();    \
            /* resend - shadow_copy & unicast */                                    \
            (0 &&& NODE_SIZE - 1, _, 0) : get_shadow_copy_##i##_##j##_slot_0_act(); \
            (0 &&& NODE_SIZE - 1, _, 1) : get_shadow_copy_##i##_##j##_slot_1_act(); \
            /* incomplete */                                                        \
            /* new pkt - reduce */                                                  \
            (                  _, 0, 0) : reduce_##i##_##j##_slot_0_act();          \
            (                  _, 0, 1) : reduce_##i##_##j##_slot_1_act();          \
            /* resend - do nothing */                                               \
            (                  _, _, _) : NoAction();                               \
        }                                                                           \
                                                                                    \
        const default_action = NoAction;                                            \
    }

#define REDUCE_TABLE_STAGE(i) \
    REDUCE_TABLE(i, 0)        \
    REDUCE_TABLE(i, 1)        \
    REDUCE_TABLE(i, 2)        \
    REDUCE_TABLE(i, 3)

#define REDUCE_STAGE(i)    \
    REDUCE_REG_STAGE(i);   \
    REDUCE_ACTION_STAGE(i) \
    REDUCE_TABLE_STAGE(i)

#define REDUCE_TABLE_APPLY(i)     \
    reduce_table_##i##_0.apply(); \
    reduce_table_##i##_1.apply(); \
    reduce_table_##i##_2.apply(); \
    reduce_table_##i##_3.apply()

#define REDUCE(i)             \
        reduce_##i##_0_act(); \
        reduce_##i##_1_act(); \
        reduce_##i##_2_act(); \
        reduce_##i##_3_act()

#define REDUCE_RESET(i)             \
        reduce_reset_##i##_0_act(); \
        reduce_reset_##i##_1_act(); \
        reduce_reset_##i##_2_act(); \
        reduce_reset_##i##_3_act()

#define GET_SHADOW_COPY(i)             \
        get_shadow_copy_##i##_0_act(); \
        get_shadow_copy_##i##_1_act(); \
        get_shadow_copy_##i##_2_act(); \
        get_shadow_copy_##i##_3_act()

control Reduce(
    in    bit<1> slot,
    in    bit<IDX_WIDTH> index,
    in    PortId_t ingress_port,
    inout bit<MAP_WIDTH> bitmap,
    inout reduce_data_t data,
    inout my_ingress_metadata_t meta)
{
    CheckBitmap() check_bitmap;

    RegisterCounter() reduce_counter;

    REDUCE_STAGE(0)
    REDUCE_STAGE(1)
    REDUCE_STAGE(2)
    REDUCE_STAGE(3)
    // REDUCE_STAGE(4)
    // REDUCE_STAGE(5)
    // REDUCE_STAGE(6)
    // REDUCE_STAGE(7)

    apply
    {
        // stage 0 1
        check_bitmap.apply(slot, index, ingress_port, bitmap, meta.status);

        // stage 2
        reduce_counter.apply(slot, index, meta.status, meta.counter_value);

        // stage 3 4 5 6 7 8 9 10
        REDUCE_TABLE_APPLY(0);
        REDUCE_TABLE_APPLY(1);
        REDUCE_TABLE_APPLY(2);
        REDUCE_TABLE_APPLY(3);
        // REDUCE_TABLE_APPLY(4);
        // REDUCE_TABLE_APPLY(5);
        // REDUCE_TABLE_APPLY(6);
        // REDUCE_TABLE_APPLY(7);
    }
}


#endif //_REDUCE_P4_
