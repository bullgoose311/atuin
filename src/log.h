#pragma once

#include <stdarg.h>

enum LogLabel_e
{
	LOG_LABEL_DEFAULT,
	LOG_LABEL_DEBUG,
	LOG_LABEL_SOCKETS,
	LOG_LABEL_MEMORY,
	LOG_LABEL_NETWORK_CLIENT,
	LOB_LABEL_NETWORK_SERVER
};

void Log_Info(LogLabel_e label, const char* format, ...);
void Log_Error(LogLabel_e label, const char* fmt, va_list args);
void Log_Error(LogLabel_e label, const char* format, ...);