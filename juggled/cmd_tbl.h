#ifndef __CMD_TBL_H__
#define __CMD_TBL_H__

#include "cmd.h"
#include "main.h"

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
    READ_WRITE_FLAG__CMD_TBL(read_adxl_flag),
    READ_WRITE_FLAG__CMD_TBL(adxl_flag),
    READ_WRITE_FLAG__CMD_TBL(print_flag),
    READ_WRITE_FLAG__CMD_TBL(transmit_flag),
    {NULL,      NULL}
};

#endif // __CMD_TBL_H__

