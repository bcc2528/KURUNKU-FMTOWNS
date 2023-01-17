#ifndef	___COMMON_H___
#define	___COMMON_H___

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <conio.h>
#include <types.h>
#include <timeb.h>
#include <egb.h>
#include <spr.h>
#include <snd.h>
#include <mos.h>
#include <his.h>
#include <dos.h>
#include <msdos.cf>


typedef char		int8_t;
typedef unsigned char	uint8_t;
typedef short		int16_t;
typedef unsigned short	uint16_t;
typedef long		int32_t;
typedef unsigned long	uint32_t;
typedef enum {
    false,  // 0
    true,  // 1
} bool;


enum
{
	X	= 0,
	Y,
	XY,
};

#define PI 3.1415926535

#define	RGB(_r, _g, _b)		(((uint16_t)(_g) << 10) | ((uint16_t)(_r) << 5) | ((uint16_t)(_b)))

#endif
