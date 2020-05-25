#include "head.h"
#include <stdarg.h>

void trace(const char *  format, ...){
	if(t != 0)
	{
		va_list ap;
		va_start(ap, format);
		vprintf(format, ap);
		va_end(ap);
	}	
}
