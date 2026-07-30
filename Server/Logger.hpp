#pragma once
#include <fstream>
#include <string>
#include <chrono>
#include <ctime>
#include <mutex>
#include <iostream>

class Logger {
public:
    static Logger& instance() {
        static Logger logger;
        return logger;
    }

    void log(const std::string& direction, int sessionId, const std::string& message) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::string entry = "[" + timestamp() + "] [" + direction + "] session=" +
                            std::to_string(sessionId) + " " + message;
        if (file_.is_open()) file_ << entry << "\n" << std::flush;
    }

    void openFile(const std::string& path) {
        std::lock_guard<std::mutex> lock(mutex_);
        file_.open(path, std::ios::app);
    }

private:
    Logger() = default;
    std::string timestamp() {
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        char buf[20];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
        return buf;
    }
    std::ofstream file_;
    std::mutex mutex_;
};

#define LOG_RECV(sessionId, msg) Logger::instance().log("RECV", sessionId, msg)
#define LOG_SEND(sessionId, msg) Logger::instance().log("SEND", sessionId, msg)
#define LOG_EVENT(msg)           Logger::instance().log("EVENT", 0, msg)