//
// Created by liqiang on 2025/9/2.
//

#ifndef RENDERER_LOGGER_H
#define RENDERER_LOGGER_H
#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <memory>

class Logger {
public:
    static void init();
    static std::shared_ptr<spdlog::logger>& getConsoleLogger() { return s_consoleLogger; }
    static std::shared_ptr<spdlog::logger>& getFileLogger() { return s_fileLogger; }

private:
    static std::shared_ptr<spdlog::logger> s_consoleLogger;
    static std::shared_ptr<spdlog::logger> s_fileLogger;
};

#define LOG_TRACE(...)    Logger::getConsoleLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)     Logger::getConsoleLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)     Logger::getConsoleLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)    Logger::getConsoleLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) Logger::getConsoleLogger()->critical(__VA_ARGS__)

#define FLOG_INFO(...)    Logger::getFileLogger()->info(__VA_ARGS__)
#define FLOG_ERROR(...)   Logger::getFileLogger()->error(__VA_ARGS__)

#endif //RENDERER_LOGGER_H