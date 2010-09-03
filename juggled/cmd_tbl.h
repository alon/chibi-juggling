#ifndef __CMD_TBL_H__
#define __CMD_TBL_H__

#include "cmd.h"
#include "juggled_main.h"

static cmd_t cmd_tbl[] = 
{
    {"read",    cmd_reg_read},
    {"write",   cmd_reg_write},
    {"ssa",     cmd_set_short_addr},
    {"gsa",     cmd_get_short_addr},
    {"who",     cmd_who},
    {"csent",   cmd_print_adxl_transmit_count},
    {"cread",   cmd_print_adxl_read_count},
    {"send",    cmd_send_test_message},
    {"enb",     cmd_enable_adxl_interrupt},
    READ_WRITE_FLAG__CMD_TBL(read_adxl),
    READ_WRITE_FLAG__CMD_TBL(print),
    READ_WRITE_FLAG__CMD_TBL(transmit),
    {NULL,      NULL}
};

#endif // __CMD_TBL_H__

