#pragma once

#include <cqcppsdk/cqcppsdk.h>

#include <memory>
#include <type_traits>

namespace xiaofan {
    template <typename E, typename = std::enable_if<std::is_base_of_v<cq::UserEvent, E>>>
    struct Session {
        const E &event;

        template <typename T, typename = std::enable_if_t<std::is_base_of_v<E, T>>>
        explicit Session(const T &event) : event(event) {
        }
    };

    using MessageSession = Session<cq::MessageEvent>;
    using NoticeSession = Session<cq::NoticeEvent>;
    using RequestSession = Session<cq::RequestEvent>;
} // namespace xiaofan
