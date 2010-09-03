#ifndef __CMD_TBL_H__
#define __CMD_TBL_H__

#include "cmd.h"
#include "stationary_main.h"

static cmd_t cmd_tbl[] = 
{
    {"enb",     cmd_enb_print},
    {"ssa",     cmd_set_short_addr},
    {"gsa",     cmd_get_short_addr},
    {"who",     cmd_who},
    {"isrec",   cmd_is_receiving},
    {"recv",    cmd_recv},
    {NULL,      NULL}
};

#endif // __CMD_TBL_H__

