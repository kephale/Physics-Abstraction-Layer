#ifndef STATUSCODE_H
#define STATUSCODE_H
//(c) Adrian Boeing 2004, see liscence.txt (BSD liscence)
/**
	Abstract:
		Valid status codes for function signaling (error return values)
	Author: 
		Adrian Boeing
	Revision History:
		Version 1.0 : 29/09/03
	TODO:
		Make independent of limits.h if sensible
*/

#define STATUSCODE_VERSION 1.0

#include <limits.h>

#ifndef STATUS_CODES
#define STATUS_CODES
typedef enum {
	SOK     = 0,
	SMESSAGE= 1,
	SWARNING= (((~INT_MAX) >> 1) | (~INT_MAX)) ^ (~INT_MAX), // a bit complex, but needed because >>1 wont ensure 1 or 0
	SDEBUG  = 2,
	SERROR = ~INT_MAX,
	SCRITICALERROR = (~INT_MAX)|((~INT_MAX)>>1),
	//SERROR  = -2147483648,
	//SCRITICALERROR = -1073741824,
	SUNSUPPORTED = ~1^(INT_MAX),
	SUNINIT = ~2^(INT_MAX),
	SNOMEM  = ~3,//^(INT_MAX),
	SUNKOWN = ~4^(INT_MAX),
	SNOTFOUND  = ~5^(INT_MAX), //eg:no API
	SINVALIDPARAM = ~6^(INT_MAX),
	SRANGE	= ~7^(INT_MAX) //eg overflow/underflow
	
} StatusCode;

#define TEST_WARNING (SWARNING)
#define TEST_ERROR (SERROR)
#define TEST_CRITICAL_ERROR (SCRITICALERROR & (~SERROR))

#endif


#endif

