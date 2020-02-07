#pragma once

#include <cqcppsdk/cqcppsdk.h>

#include <memory>
#include <type_traits>

namespace xiaofan {
    template <typename E, typename = std::enable_if<std::is_base_of_v<cq::UserEvent, E>>>
    struct Session {
        std::shared_ptr<E> event;

        template <typename T, typename = std::enable_if_t<std::is_base_of_v<E, T>>,
                  typename = std::enable_if_t<std::is_final_v<T>> /* need to copy event here, so must be final class */>
        explicit Session(const T &event) : event(std::make_shared<T>(event)) {
        }
    };

    using MessageSession = Session<cq::MessageEvent>;
    using NoticeSession = Session<cq::NoticeEvent>;
    using RequestSession = Session<cq::RequestEvent>;
} // namespace xiaofan
