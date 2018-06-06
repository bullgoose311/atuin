#pragma once

#include <stdarg.h>

enum LogLabel_e
{
	LOG_LABEL_DEFAULT,
	LOG_LABEL_SOCKETS
};

void Log_Info(LogLabel_e label, const char* format, ...);
void Log_Error(LogLabel_e label, const char* fmt, va_list args);
void Log_Error(LogLabel_e label, const char* format, ...);