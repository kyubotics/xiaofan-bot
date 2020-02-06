#pragma once

#include <cqcppsdk/cqcppsdk.h>

#include <atomic>
#include <map>
#include <memory>
#include <string>
#include <type_traits>

#include "message.hpp"

namespace xiaofan {
    struct App {
        std::atomic<bool> running = false;
        std::map<std::string, std::shared_ptr<message::MessageHandler>> message_handlers;
    };

    extern App app;

    void startup();
    void shutdown();

    inline bool add_message_handler(const std::string &name, const std::shared_ptr<message::MessageHandler> &handler) {
        app.message_handlers[name] = handler;
        return true;
    }

    template <typename E, typename = std::enable_if_t<std::is_base_of_v<cq::MessageEvent, E>>>
    inline void handle_message(const E &event) {
        for (const auto &item : app.message_handlers) {
            const auto &name = item.first;
            if (string::startswith(name, "_")) continue;

            const auto &handler = item.second;
            if (handler->check_condition(event)) {
                handler->run(event);
                if (event.blocked()) break;
            }
        }
    }
} // namespace xiaofan

#define MESSAGE_HANDLER(Name, ...)                                                            \
    static void __dummy_message_handler_##Name(xiaofan::message::MessageSession &);           \
    static const auto __dummy_message_handler_##Name##_res =                                  \
        xiaofan::add_message_handler(#Name,                                                   \
                                     xiaofan::message::MessageHandlerBuilder()                \
                                         .condition(xiaofan::message::cond::All(__VA_ARGS__)) \
                                         .handler(__dummy_message_handler_##Name)             \
                                         .build());                                           \
    static void __dummy_message_handler_##Name(xiaofan::message::MessageSession &session)
