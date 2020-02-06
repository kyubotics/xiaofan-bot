#include "app.hpp"

namespace xiaofan {
    static const char *TAG = "应用";

    App app;

    void startup() {
        if (app.running) return;

        app.running = true;
        cq::logging::info(TAG, "开工！");
    }

    void shutdown() {
        if (!app.running) return;

        app.running = false;
        cq::logging::info(TAG, "下班！");
    }
} // namespace xiaofan
