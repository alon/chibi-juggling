#ifndef __ADXL_H__
#define __ADXL_H__

#include <types.h>
#include "juggled_main.h"

extern U16 acc_X, acc_Y, acc_Z;
extern volatile bool is_adxl_on;

void adxl_off();
void read_adxl(U8* dat); // must be size 6!
void adxl_init();

#endif // __ADXL_H__

