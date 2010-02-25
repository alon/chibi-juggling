#ifndef __JUGGLING_H__
#define __JUGGLING_H__

#define STATIONARY_SHORT_ADDRESS 5
#define JUGGLED_SHORT_ADDRESS 7
// 6 bytes for accel{x,y,z} and magic is 2 bytes
#define ADXL_PACKET_LENGTH (6 + ADXL_MAGIC_LENGTH)
#define ADXL_MAGIC_LENGTH 2
#define ADXL_MAGIC "JG"
#define ADXL_MAGIC_BYTE_1 'J'
#define ADXL_MAGIC_BYTE_2 'G'

#endif //__JUGGLING_H__

