#ifndef __CMD_TBL_H__
#define __CMD_TBL_H__

#include "cmd.h"
#include "main.h"

static cmd_t cmd_tbl[] = 
{
    {"enb",     cmd_enb_print},
    {"ssa",     cmd_set_short_addr},
    {"gsa",     cmd_get_short_addr},
    {NULL,      NULL}
};

#endif // __CMD_TBL_H__

