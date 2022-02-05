#pragma once
#include <stdio.h>

#define dfLOG_LEVEL_DEBUG 0
#define dfLOG_LEVEL_WARNING 1
#define dfLOG_LEVEL_ERROR 2

#define _LOG(LogLevel, fmt, ...)	\
	do{	\
		if(g_LogLevel <= LogLevel) { \
			swprintf_s(g_wsLogBuf,fmt,##__VA_ARGS__);\
			Log(g_wsLogBuf,LogLevel);\
		}\
	} while (0) 

#define CRASH() do{\
		_LOG(dfLOG_LEVEL_DEBUG, L"///////CRASH : %s [%d]",__FILEW__,__LINE__);\
		int *nptr = nullptr; *nptr = 1;\
	}while(0)


extern int g_LogLevel;
extern wchar_t g_wsLogBuf[256];
extern char g_sFilename[256];


void Log(wchar_t *wsString, int iLogLevel);