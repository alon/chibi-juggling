#ifndef __CMD_TBL_H__
#define __CMD_TBL_H__

#include "cmd.h"
#include "main.h"

static cmd_t cmd_tbl[] = 
{
    {"enb",     cmd_enb_print},
    {"tran",    cmd_enb_transmit},
    {"adxl",    cmd_enb_adxl},
    {"read",    cmd_reg_read},
    {"write",   cmd_reg_write},
    {"ssa",     cmd_set_short_addr},
    {"gsa",     cmd_get_short_addr},
    {NULL,      NULL}
};

#endif // __CMD_TBL_H__

