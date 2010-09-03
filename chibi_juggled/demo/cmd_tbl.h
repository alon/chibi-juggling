#include "cmd.h"
#include "types.h"
#include "chb_demo.h"

// command table
chb_cmd_t cmd_tbl[] = 
{
    {"hello",   cmd_hello},
    {"setsaddr", cmd_set_short_addr},
    {"getsaddr", cmd_get_short_addr},
    {"setiaddr", cmd_set_ieee_addr},
    {"getiaddr", cmd_get_ieee_addr},
    {"rd", cmd_reg_read},
    {"wr", cmd_reg_write},
    {"dump", cmd_reg_dump},
    {"send", cmd_tx},

#ifdef CHB_AT86RF212
    {"mode", cmd_set_mode},
    {"pwr", cmd_set_pwr},
#endif

    {NULL,      NULL}
};
