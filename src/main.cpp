#include <algorithm>
#include <dolores/dolores.hpp>
#include <regex>

using namespace cq;
using namespace cq::message;
using namespace dolores;
using namespace dolores::cond;

constexpr int64_t SUPERUSER_ID = 1002647525;

#ifdef _DEBUG
constexpr int64_t RELEASE_GROUP_ID = 615346135;
#else
constexpr int64_t RELEASE_GROUP_ID = 218529254;
#endif

dolores_on_request(approve_invitation, group() & user({SUPERUSER_ID})) {
    try {
        session.approve();
    } catch (ApiError &) {
    }
    session.event.block();
}

dolores_on_notice(group_admin, type<GroupAdminEvent>, group::exclude({RELEASE_GROUP_ID})) {
    if (session.event.user_id == get_login_user_id()
        && session.event_as<GroupAdminEvent>().sub_type == GroupAdminEvent::SubType::SET) {
        try {
            session.send("感谢群主大佬提拔~");
        } catch (ApiError &) {
        }
    }
}

dolores_on_notice(welcome_new_member, type<GroupMemberIncreaseEvent>, group::exclude({RELEASE_GROUP_ID})) {
    const auto &event = session.event_as<GroupMemberIncreaseEvent>();
    try {
        const auto mi = get_group_member_info(event.group_id, get_login_user_id());
        if (mi.role == GroupRole::MEMBER) {
            return;
        }
        try {
            session.send("欢迎新群友👏👏", true);
        } catch (ApiError &) {
        }
    } catch (ApiError &) {
    }
}

dolores_on_message(ban, command({"ban", "b"}) | (startswith("小凡") & (contains("烟") | contains("禁言"))),
                   group() & admin()) {
    const auto &event = session.event_as<GroupMessageEvent>();
    event.block();

    try {
        const auto mi = get_group_member_info(event.group_id, get_login_user_id(), true);
        if (mi.role == GroupRole::MEMBER) {
            try {
                session.send("我还不是管理员，禁不了！");
            } catch (ApiError &) {
            }
            return;
        }
    } catch (ApiError &) {
    }

    std::vector<int64_t> to_ban;

    Message msg(event.message);
    for (const auto &seg : msg) {
        if (seg.type != "at" || seg == MessageSegment::at(get_login_user_id())) {
            continue;
        }
        to_ban.push_back(std::stoll(seg.data.at("qq")));
    }

    int64_t duration = 30 * 60; // 30 minutes by default
    std::smatch m;
    std::regex_search(event.message, m, std::regex(R"((\d+)m|(\d+)h|(\d+)d)"));
    if (!m.str(1).empty()) {
        duration = std::stoll(m.str(1)) * 60;
    } else if (!m.str(2).empty()) {
        duration = std::stoll(m.str(2)) * 60 * 60;
    } else if (!m.str(3).empty()) {
        duration = std::stoll(m.str(3)) * 60 * 60 * 24;
    }

    for (const auto user_id : to_ban) {
        try {
            const auto mi = get_group_member_info(event.group_id, user_id);
            if (mi.role != GroupRole::MEMBER) {
                try {
                    session.send(MessageSegment::at(user_id) + " 这人官儿大，禁不动！");
                } catch (ApiError &) {
                }
                return;
            }
        } catch (ApiError &) {
        }
        try {
            set_group_ban(event.group_id, user_id, duration);
        } catch (ApiError &) {
            try {
                session.send("禁言 " + MessageSegment::at(user_id) + " 失败！");
            } catch (ApiError &) {
            }
        }
    }
}

dolores_on_message(cqmoe_release, command("release") & contains("更新日志"), direct({SUPERUSER_ID})) {
    /*
     * 格式如:
     *
     * 项目: xxx
     * 仓库: github.com/cqmoe/xxx
     * 版本: vx.x.x
     *
     * 更新日志：
     *
     * - xxx
     */
    session.event.block();

    auto first_space_it =
        std::find_if(session.event.message.cbegin(), session.event.message.cend(), cq::utils::isspace_s);
    if (first_space_it == session.event.message.cend()) {
        try {
            session.send("格式不太对~");
        } catch (ApiError &) {
        }
        return;
    }

    auto update_info = std::string(first_space_it, session.event.message.cend());
    cq::utils::string_trim(update_info);
    try {
        send_group_message(RELEASE_GROUP_ID, update_info);
        session.send("发布好啦！");
    } catch (ApiError &) {
    }
}
