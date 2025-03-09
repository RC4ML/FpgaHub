#ifndef _CONFIG_P4_
#define _CONFIG_P4_


const int SLOT_SIZE = 16384; // 22528; // 143360 (= 35 * 4096) maximum with 32 bits for single stage, 4 meter ALU as most 

#define IDX_WIDTH 14 // 2 ^ IDX_WIDTH == SLOT_SIZE
#define IDX_PADDING_WIDTH 17 // = 32 - 1 (slot) - IDX_WIDTH
#define HDR_PADDING_WIDTH 416 // 512 - 32 - 32 - 32

#define VAL_WIDTH 32

#define MAP_WIDTH 32

#define CNT_WIDTH 32

#define NODE_SIZE 1


#endif //_CONFIG_P4_
