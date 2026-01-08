#ifndef METRICS_H
#define METRICS_H

#include <atomic>
#include <chrono>

struct AppMetrics
{
    std::atomic<long> total_requests{0};
    std::atomic<long> total_errors{0};
    std::atomic<long> successful_requests{0};

    std::chrono::time_point<std::chrono::steady_clock> start_time;

    AppMetrics() { start_time = std::chrono::steady_clock::now(); }

    void register_request() { total_requests++; }

    void register_error() { total_errors++; }

    void register_success() { successful_requests++; }

    long get_uptime_seconds()
    {
        auto now = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::seconds>(now -
                                                                start_time)
            .count();
    }
};

static AppMetrics server_metrics;

#endif
