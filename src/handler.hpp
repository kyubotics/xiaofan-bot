#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

#include "condition.hpp"
#include "session.hpp"
#include "string.hpp"
#include "traits.hpp"

namespace xiaofan {
    template <typename E, typename = enable_if_derived_from_user_event_t<E>>
    class Handler {
    public:
        explicit Handler(std::function<void(Session<E> &session)> impl, std::shared_ptr<Condition> condition = nullptr)
            : _impl(std::move(impl)), _condition(std::move(condition)) {
        }

        [[nodiscard]] bool check_condition(const E &event) const {
            if (!_condition) return true;
            return (*_condition)(event);
        }

        void run(Session<E> &session) const {
            if (!_impl) return;
            _impl(session);
        }

    private:
        std::shared_ptr<Condition> _condition;
        std::function<void(Session<E> &session)> _impl;
    };

    extern struct HandlerMapWrapper {
        template <typename E, typename = enable_if_derived_from_user_event_t<E>>
        using HandlerMap = std::map<std::string, std::shared_ptr<Handler<E>>>;

        HandlerMap<cq::MessageEvent> message_handlers;
        HandlerMap<cq::NoticeEvent> notice_handlers;
        HandlerMap<cq::RequestEvent> request_handlers;
    } _handler_map_wrapper;

    template <typename E, typename = enable_if_derived_from_user_event_t<E>>
    constexpr auto &get_handlers() {
        if constexpr (std::is_base_of_v<cq::MessageEvent, E>) {
            return _handler_map_wrapper.message_handlers;
        } else if constexpr (std::is_base_of_v<cq::NoticeEvent, E>) {
            return _handler_map_wrapper.notice_handlers;
        } else { // std::is_base_of_v<cq::RequestEvent, E>
            return _handler_map_wrapper.request_handlers;
        }
    }

    template <typename E, typename = enable_if_derived_from_user_event_t<E>>
    inline constexpr bool add_handler(const std::string &name, const std::shared_ptr<Handler<E>> &handler) {
        auto &handlers = get_handlers<E>();
        handlers[name] = handler;
        return true;
    }

    template <typename E, typename = enable_if_derived_from_user_event_t<E>>
    inline void run_handlers(const E &event) {
        const auto &handlers = get_handlers<E>();
        for (const auto &item : handlers) {
            const auto &name = item.first;
            if (string::startswith(name, "_")) continue;

            const auto &handler = item.second;
            if (handler->check_condition(event)) {
                if constexpr (std::is_base_of_v<cq::MessageEvent, E>) {
                    MessageSession session(event);
                    handler->run(session);
                } else if constexpr (std::is_base_of_v<cq::NoticeEvent, E>) {
                    NoticeSession session(event);
                    handler->run(session);
                } else { // std::is_base_of_v<cq::RequestEvent, E>
                    RequestSession session(event);
                    handler->run(session);
                }
            }
        }
    }
} // namespace xiaofan

#define MESSAGE_HANDLER(Name, ...)                                                                                    \
    static void __dummy_message_handler_##Name(xiaofan::MessageSession &);                                            \
    static const auto __dummy_message_handler_##Name##_res =                                                          \
        xiaofan::add_handler(#Name,                                                                                   \
                             std::make_shared<xiaofan::Handler<cq::MessageEvent>>(                                    \
                                 __dummy_message_handler_##Name, std::make_shared<xiaofan::cond::All>(__VA_ARGS__))); \
    static void __dummy_message_handler_##Name(xiaofan::MessageSession &session)

#define NOTICE_HANDLER(Name, ...)                                                                                    \
    static void __dummy_notice_handler_##Name(xiaofan::NoticeSession &);                                             \
    static const auto __dummy_notice_handler_##Name##_res =                                                          \
        xiaofan::add_handler(#Name,                                                                                  \
                             std::make_shared<xiaofan::Handler<cq::NoticeEvent>>(                                    \
                                 __dummy_notice_handler_##Name, std::make_shared<xiaofan::cond::All>(__VA_ARGS__))); \
    static void __dummy_notice_handler_##Name(xiaofan::NoticeSession &session)

#define REQUEST_HANDLER(Name, ...)                                                                                    \
    static void __dummy_request_handler_##Name(xiaofan::RequestSession &);                                            \
    static const auto __dummy_request_handler_##Name##_res =                                                          \
        xiaofan::add_handler(#Name,                                                                                   \
                             std::make_shared<xiaofan::Handler<cq::RequestEvent>>(                                    \
                                 __dummy_request_handler_##Name, std::make_shared<xiaofan::cond::All>(__VA_ARGS__))); \
    static void __dummy_request_handler_##Name(xiaofan::RequestSession &session)
