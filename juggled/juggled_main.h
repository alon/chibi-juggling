/*******************************************************************/
/*!
    \file main.h
    \ingroup cdc_demo
*/
/*******************************************************************/
#ifndef MAIN_H
#define MAIN_H

// This acts as the "config.h" right now -i.e. please don't create any
// circular dependencies, try to minimize what goes in here and make it
// a leaf node.

#include <avr/io.h>
#include <types.h>
void cmd_reg_read(U8 argc, char **argv);
void cmd_reg_write(U8 argc, char **argv);
void cmd_set_short_addr(U8 argc, char **argv);
void cmd_get_short_addr(U8 argc, char **argv);
void cmd_print_adxl_transmit_count(U8 argc, char **argv);
void cmd_print_adxl_read_count(U8 argc, char **argv);
void cmd_enable_adxl_interrupt(U8 argc, char **argv);

// Test / Debugging commands
void cmd_who(U8 argc, char **argv);
void cmd_send_test_message(U8 argc, char **argv);

// Some helpers to make the code slightly easier to read
#define READ_WRITE_FLAG__CMD_IMPL(flag) \
void cmd_##flag##_on (U8 argc, char **argv) \
{ \
 flag = true;\
} \
void cmd_##flag##_off (U8 argc, char **argv) \
{ \
 flag = false;\
}\
void cmd_##flag##_show (U8 argc, char **argv) \
{ \
    printf_P(PSTR(#flag" %d\n"), flag);\
}


#define READ_WRITE_FLAG__FLAG_IMP(flag) \
static bool flag;

#define READ_WRITE_FLAG__FLAG_IMP_DEFAULT(flag, def) \
static bool flag = def;


#define READ_WRITE_FLAG__CMD_DECL(flag) \
void cmd_##flag##_on (U8 argc, char **argv); \
void cmd_##flag##_off (U8 argc, char **argv); \
void cmd_##flag##_show (U8 argc, char **argv)

#define READ_WRITE_FLAG__CMD_TBL(flag) \
 {#flag "_on", cmd_##flag##_on},\
 {#flag "_off", cmd_##flag##_off},\
 {#flag "_show", cmd_##flag##_show}

READ_WRITE_FLAG__CMD_DECL(read_adxl);
READ_WRITE_FLAG__CMD_DECL(print);
READ_WRITE_FLAG__CMD_DECL(transmit);

#endif

