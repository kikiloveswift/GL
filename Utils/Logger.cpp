//
// Created by liqiang on 2025/9/2.
//

#include "Logger.h"
#include "Logger.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include <iostream>
#include <filesystem>

std::shared_ptr<spdlog::logger> Logger::s_consoleLogger;
std::shared_ptr<spdlog::logger> Logger::s_fileLogger;

void Logger::init() {
    // 避免重复初始化
    if (s_consoleLogger) {
        return;
    }

    try {
        // 创建logs目录
        std::filesystem::create_directories("logs");

        // 创建控制台sink
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

        // 创建文件sink
        auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            "logs/file.log", 1024*1024*5, 3);

        // 手动创建logger (避免自动注册冲突)
        s_consoleLogger = std::make_shared<spdlog::logger>("console", console_sink);
        s_fileLogger = std::make_shared<spdlog::logger>("file", file_sink);

        // 配置
        s_consoleLogger->set_level(spdlog::level::trace);
        s_consoleLogger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");

        s_fileLogger->set_level(spdlog::level::info);
        s_fileLogger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%t] %v");

        // 注册到spdlog
        spdlog::register_logger(s_consoleLogger);
        spdlog::register_logger(s_fileLogger);

        spdlog::set_default_logger(s_consoleLogger);

    } catch (const spdlog::spdlog_ex& ex) {
        std::cerr << "Logger initialization failed: " << ex.what() << std::endl;
    }
}