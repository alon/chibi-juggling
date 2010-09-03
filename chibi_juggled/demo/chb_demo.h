/*******************************************************************
    Copyright (C) 2009 FreakLabs
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    1. Redistributions of source code must retain the above copyright
       notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
       notice, this list of conditions and the following disclaimer in the
       documentation and/or other materials provided with the distribution.
    3. Neither the name of the the copyright holder nor the names of its contributors
       may be used to endorse or promote products derived from this software
       without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS'' AND
    ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
    OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
    OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.

    Originally written by Christopher Wang aka Akiba.
    Please post support questions to the FreakLabs forum.

*******************************************************************/
/*!
    \file 
    \ingroup


*/
/**************************************************************************/
#ifndef CHB_DEMO_H
#define CHB_DEMO_H

#include <stdio.h>
#include <stdlib.h>
#include "types.h"

typedef struct
{
    char *cmd;
    void (*func)(U8 argc, char **argv);
} chb_cmd_t;

void cmd_hello(U8 argc, char **argv);
void cmd_set_short_addr(U8 argc, char **argv);
void cmd_get_short_addr(U8 argc, char **argv);
void cmd_set_ieee_addr(U8 argc, char **argv);
void cmd_get_ieee_addr(U8 argc, char **argv);
void cmd_tx(U8 argc, char **argv);
void cmd_reg_read(U8 argc, char **argv);
void cmd_reg_write(U8 argc, char **argv);
void cmd_reg_dump(U8 argc, char **argv);

#ifdef CHB_AT86RF212
void cmd_set_mode(U8 argc, char **argv);
void cmd_set_pwr(U8 argc, char **argv);
#endif

#endif
