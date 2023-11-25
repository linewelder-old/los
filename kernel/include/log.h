#pragma once

void log(const char* severity, const char* file, int line, const char* format, ...);

#define LOG_INFO(...) log("INF", __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...) log("WRN", __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) log("ERR", __FILE__, __LINE__, __VA_ARGS__)
