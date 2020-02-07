#pragma once

#include <functional>
#include <memory>
#include <type_traits>
#include <vector>

#include "condition.hpp"
#include "session.hpp"
#include "string.hpp"

namespace xiaofan {
    template <typename E>
    using enable_if_is_user_event_t = std::enable_if_t<std::is_base_of_v<cq::UserEvent, E>>;

    template <typename E, typename = enable_if_is_user_event_t<E>>
    class Handler {
    public:
        [[nodiscard]] bool check_condition(const E &event) const {
            for (const auto &condition : conditions) {
                if (!(*condition)(event)) return false; // conditions must be fully matched
            }
            return !conditions.empty();
        }

        void run(Session<E> &session) const {
            if (!impl) return;
            impl(session);
        }

        std::vector<std::shared_ptr<Condition>> conditions;
        std::function<void(Session<E> &session)> impl;
    };

    using MessageHandler = Handler<cq::MessageEvent>;
    using NoticeHandler = Handler<cq::NoticeEvent>;
    using RequestHandler = Handler<cq::RequestEvent>;

    template <typename E, typename = enable_if_is_user_event_t<E>>
    class HandlerBuilder {
    public:
        HandlerBuilder() {
            _handler = std::shared_ptr<Handler<E>>(new Handler<E>);
        }

        template <typename T, typename = enable_if_is_condition_t<T>>
        HandlerBuilder &&condition(const T &cond) && {
            _handler->conditions.push_back(std::make_shared<T>(cond));
            return std::move(*this);
        }

        HandlerBuilder &&handler(const std::function<void(Session<E> &session)> &handler) && {
            _handler->impl = handler;
            return std::move(*this);
        }

        std::shared_ptr<Handler<E>> build() && {
            return _handler;
        }

    private:
        std::shared_ptr<Handler<E>> _handler;
    };

    using MessageHandlerBuilder = HandlerBuilder<cq::MessageEvent>;
    using NoticeHandlerBuilder = HandlerBuilder<cq::NoticeEvent>;
    using RequestHandlerBuilder = HandlerBuilder<cq::RequestEvent>;
} // namespace xiaofan
