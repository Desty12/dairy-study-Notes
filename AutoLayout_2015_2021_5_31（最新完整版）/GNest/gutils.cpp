#include <math.h>
#include "gutils.h"

void gSwapByte4(char *to, const char *from)
{
	unsigned char tmp;
	tmp = from[0];
	to[0] = from[3];
	to[3] = tmp;
	tmp = from[1];
	to[1] = from[2];
	to[2] = tmp;
}

void gSwapByte2(char *to, const char *from)
{
	unsigned char tmp;
	tmp = from[0];
	to[0] = from[1];
	to[1] = tmp;
}
