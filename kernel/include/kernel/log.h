#pragma once

enum class SeverityLevel {
    INFO,
    WARNING,
    ERROR,
};

void log(SeverityLevel severity, const char* file, int line, const char* format, ...);

#define LOG_INFO(...) log(SeverityLevel::INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...) log(SeverityLevel::WARNING, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) log(SeverityLevel::ERROR, __FILE__, __LINE__, __VA_ARGS__)
