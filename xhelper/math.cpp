#include "include\xhelper.h"
#include <math.h>
#define PI 3.14159265

XHELPER_API float cosminusone(float v) {
	return acosf(v) * 180.0 / PI;
}

XHELPER_API float tanminusone(float v) {
	return atanf(v) * 180.0 / PI;
}

XHELPER_API float cosn(float v) {
	return cosf(v * PI / 180.0);
}

XHELPER_API float sinn(float v) {
	return sinf(v * PI / 180.0);
}

XHELPER_API float roundn(float i)
{
	return roundf(i);
}
