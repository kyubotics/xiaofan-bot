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

    on_private_message(handle_event<PrivateMessageEvent>);
    on_group_message(handle_event<GroupMessageEvent>);

    on_group_admin(handle_event<GroupAdminEvent>);
    on_group_member_increase(handle_event<GroupMemberIncreaseEvent>);

    on_group_request(handle_event<GroupRequestEvent>);
}
