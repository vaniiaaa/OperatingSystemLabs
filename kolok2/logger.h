#ifndef LOGGER_H
#define LOGGER_H

#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>

std::mutex log_mutex;

inline void log_message(const std::string &message)
{
    std::lock_guard<std::mutex> lock(log_mutex);

    std::ofstream log_file("server.log", std::ios_base::app);

    if (log_file.is_open())
    {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);

        std::string timestr = std::ctime(&now_time);
        if (!timestr.empty() && timestr.back() == '\n')
        {
            timestr.pop_back();
        }

        log_file << "[" << timestr << "] " << message << std::endl;
    }
    else
    {
        std::cerr << "File not opened" << std::endl;
    }
}

#endif
