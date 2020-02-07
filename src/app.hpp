#pragma once

#include <cqcppsdk/cqcppsdk.h>

#include <atomic>
#include <map>
#include <memory>
#include <string>
#include <type_traits>

#include "condition.hpp"
#include "handler.hpp"
#include "session.hpp"

namespace xiaofan {
    struct App {
        std::atomic<bool> running = false;

        template <typename E, typename = enable_if_is_user_event_t<E>>
        using HandlerMap = std::map<std::string, std::shared_ptr<Handler<E>>>;

        HandlerMap<cq::MessageEvent> message_handlers;
        HandlerMap<cq::NoticeEvent> notice_handlers;
        HandlerMap<cq::RequestEvent> request_handlers;

        template <typename E, typename = enable_if_is_user_event_t<E>>
        constexpr auto &handlers() {
            if constexpr (std::is_base_of_v<cq::MessageEvent, E>) {
                return message_handlers;
            } else if constexpr (std::is_base_of_v<cq::NoticeEvent, E>) {
                return notice_handlers;
            } else { // std::is_base_of_v<cq::RequestEvent, E>
                return request_handlers;
            }
        }
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

    template <typename E, typename = enable_if_is_user_event_t<E>>
    inline constexpr bool add_handler(const std::string &name, const std::shared_ptr<Handler<E>> &handler) {
        auto &handlers = app.handlers<E>();
        handlers[name] = handler;
        return true;
    }

    template <typename E, typename = enable_if_is_user_event_t<E>,
              typename = std::enable_if_t<std::is_final_v<E>> /* required by constructor of Session */>
    inline void handle_event(const E &event) {
        const auto &handlers = app.handlers<E>();
        for (const auto &item : handlers) {
            const auto &name = item.first;
            if (string::startswith(name, "_")) continue;

            const auto &handler = item.second;
            if (handler->check_condition(event)) {
                if constexpr (std::is_base_of_v<cq::MessageEvent, E>) {
                    Session<cq::MessageEvent> session(event);
                    handler->run(session);
                    event.operation = session.event->operation;
                } else if constexpr (std::is_base_of_v<cq::NoticeEvent, E>) {
                    Session<cq::NoticeEvent> session(event);
                    handler->run(session);
                    event.operation = session.event->operation;
                } else { // std::is_base_of_v<cq::RequestEvent, E>
                    Session<cq::RequestEvent> session(event);
                    handler->run(session);
                    event.operation = session.event->operation;
                }
                if (event.blocked()) break;
            }
        }
    }
} // namespace xiaofan

#define MESSAGE_HANDLER(Name, ...)                                           \
    static void __dummy_message_handler_##Name(xiaofan::MessageSession &);   \
    static const auto __dummy_message_handler_##Name##_res =                 \
        xiaofan::add_handler(#Name,                                          \
                             xiaofan::MessageHandlerBuilder()                \
                                 .condition(xiaofan::cond::All(__VA_ARGS__)) \
                                 .handler(__dummy_message_handler_##Name)    \
                                 .build());                                  \
    static void __dummy_message_handler_##Name(xiaofan::MessageSession &session)

#define NOTICE_HANDLER(Name, ...)                                            \
    static void __dummy_notice_handler_##Name(xiaofan::NoticeSession &);     \
    static const auto __dummy_notice_handler_##Name##_res =                  \
        xiaofan::add_handler(#Name,                                          \
                             xiaofan::NoticeHandlerBuilder()                 \
                                 .condition(xiaofan::cond::All(__VA_ARGS__)) \
                                 .handler(__dummy_notice_handler_##Name)     \
                                 .build());                                  \
    static void __dummy_notice_handler_##Name(xiaofan::NoticeSession &session)

#define REQUEST_HANDLER(Name, ...)                                           \
    static void __dummy_request_handler_##Name(xiaofan::RequestSession &);   \
    static const auto __dummy_request_handler_##Name##_res =                 \
        xiaofan::add_handler(#Name,                                          \
                             xiaofan::RequestHandlerBuilder()                \
                                 .condition(xiaofan::cond::All(__VA_ARGS__)) \
                                 .handler(__dummy_request_handler_##Name)    \
                                 .build());                                  \
    static void __dummy_request_handler_##Name(xiaofan::RequestSession &session)
