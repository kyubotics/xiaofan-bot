#include <cqcppsdk/cqcppsdk.h>

#include "app.hpp"

CQ_INIT {
    using namespace cq;
    using namespace cq::message;
    using namespace xiaofan;

    on_coolq_start(startup);
    on_enable(startup);
    on_disable(shutdown);
    on_coolq_exit(shutdown);

    on_private_message(handle_message<PrivateMessageEvent>);
    on_group_message(handle_message<GroupMessageEvent>);
    on_discuss_message(handle_message<DiscussMessageEvent>);

    on_group_admin([](const GroupAdminEvent &e) {
        if (e.user_id == get_login_user_id() && e.sub_type == GroupAdminEvent::SubType::SET) {
            try {
                send_message(e.target, "感谢群主大佬提拔~");
            } catch (ApiError &) {
            }
        }
    });

    on_group_member_increase([](const GroupMemberIncreaseEvent &e) {
        try {
            const auto mi = get_group_member_info(e.group_id, get_login_user_id());
            if (mi.role == GroupRole::MEMBER) {
                return;
            }
            try {
                send_message(e.target, "欢迎新群友👏👏", true);
            } catch (ApiError &) {
            }
        } catch (ApiError &) {
        }
    });
}
