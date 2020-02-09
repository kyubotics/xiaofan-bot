#pragma once

#include <cqcppsdk/cqcppsdk.h>

#include <atomic>

namespace xiaofan {
    struct App {
        std::atomic<bool> running = false;
    };

    extern App app;

    inline void startup() {
        if (app.running) return;

        app.running = true;
        cq::logging::info("app", "开工！");
    }

    inline void shutdown() {
        if (!app.running) return;

        app.running = false;
        cq::logging::info("app", "下班！");
    }
} // namespace xiaofan
