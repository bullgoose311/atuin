#ifdef __WINDOWS__

#include "log.h"

#include "array_utils.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <Windows.h>

#define MAX_LOG_LENGTH 1024

enum LogLevel_e
{
	LOG_LEVEL_DEBUG,
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_ERROR
};

static const char* s_logLevels[] = {
	"DEBUG",
	"INFO",
	"WARNING",
	"ERROR"
};

static const char* s_logLabels[] = {
	"DEFAULT",
	"SOCKETS"
};

static void _log(LogLabel_e label, LogLevel_e level, const char* fmt, va_list args)
{
	SYSTEMTIME st;
	GetSystemTime(&st);

	char modifiedFormat[MAX_LOG_LENGTH];
	snprintf(modifiedFormat, ARRAY_COUNT(modifiedFormat), "[%d:%d:%d-%d][%s][%s] %s\n", 
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, 
		s_logLevels[level],
		s_logLabels[label], 
		fmt);

	char buffer[MAX_LOG_LENGTH];
	vsnprintf(buffer, ARRAY_COUNT(buffer), modifiedFormat, args);
	OutputDebugString(buffer);
}

void Log_Info(LogLabel_e label, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	_log(label, LOG_LEVEL_INFO, fmt, args);
	va_end(args);
}

void Log_Error(LogLabel_e label, const char* fmt, va_list args)
{
	_log(label, LOG_LEVEL_ERROR, fmt, args);
}

void Log_Error(LogLabel_e label, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	Log_Error(label, fmt, args);
	va_end(args);
}

#endif