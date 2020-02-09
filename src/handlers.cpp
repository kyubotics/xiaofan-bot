#include <cqcppsdk/cqcppsdk.h>
#include <cqcppsdk/utils/string.h>

#include <algorithm>
#include <regex>

#include "handler.hpp"

using namespace cq;
using namespace cq::message;
using namespace xiaofan;
using namespace xiaofan::cond;

constexpr int64_t SUPERUSER_ID = 1002647525;

#ifdef _DEBUG
constexpr int64_t RELEASE_GROUP_ID = 615346135;
#else
constexpr int64_t RELEASE_GROUP_ID = 218529254;
#endif

REQUEST_HANDLER(approve_invitation, group() & user({SUPERUSER_ID})) {
    auto e = dynamic_cast<const GroupRequestEvent &>(session.event);
    if (e.sub_type == GroupRequestEvent::SubType::INVITE) {
        try {
            set_group_request(e.flag, e.sub_type, RequestEvent::Operation::APPROVE);
        } catch (ApiError &) {
        }
    }
}

NOTICE_HANDLER(group_notice, group::exclude({RELEASE_GROUP_ID})) {
    switch (session.event.detail_type) {
    case NoticeEvent::DetailType::GROUP_ADMIN: {
        auto e = dynamic_cast<const GroupAdminEvent &>(session.event);
        if (e.user_id == get_login_user_id() && e.sub_type == GroupAdminEvent::SubType::SET) {
            try {
                send_message(e.target, "感谢群主大佬提拔~");
            } catch (ApiError &) {
            }
        }
        break;
    }
    case NoticeEvent::DetailType::GROUP_MEMBER_INCREASE: {
        auto e = dynamic_cast<const GroupMemberIncreaseEvent &>(session.event);
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
        break;
    }
    default:
        break;
    }
}

MESSAGE_HANDLER(ban, command("ban") | (startswith("小凡") & (contains("烟") | contains("禁言"))), group() & admin()) {
    auto e = dynamic_cast<const GroupMessageEvent &>(session.event);
    e.block();

    try {
        const auto mi = get_group_member_info(e.group_id, get_login_user_id(), true);
        if (mi.role == GroupRole::MEMBER) {
            try {
                send_message(e.target, "我还不是管理员，禁不了！");
            } catch (ApiError &) {
            }
            return;
        }
    } catch (ApiError &) {
    }

    std::vector<int64_t> to_ban;

    Message msg(e.message);
    for (const auto &seg : msg) {
        if (seg.type != "at" || seg == MessageSegment::at(get_login_user_id())) {
            continue;
        }
        to_ban.push_back(std::stoll(seg.data.at("qq")));
    }

    int64_t duration = 30 * 60; // 30 minutes by default
    std::smatch m;
    std::regex_search(e.message, m, std::regex(R"((\d+)m|(\d+)h|(\d+)d)"));
    if (!m.str(1).empty()) {
        duration = std::stoll(m.str(1)) * 60;
    } else if (!m.str(2).empty()) {
        duration = std::stoll(m.str(2)) * 60 * 60;
    } else if (!m.str(3).empty()) {
        duration = std::stoll(m.str(3)) * 60 * 60 * 24;
    }

    for (const auto user_id : to_ban) {
        try {
            const auto mi = get_group_member_info(e.group_id, user_id);
            if (mi.role != GroupRole::MEMBER) {
                try {
                    send_message(e.target, MessageSegment::at(user_id) + " 这人官儿大，禁不动！");
                } catch (ApiError &) {
                }
                return;
            }
        } catch (ApiError &) {
        }
        try {
            set_group_ban(e.group_id, user_id, duration);
        } catch (ApiError &) {
            try {
                send_message(e.target, "禁言 " + MessageSegment::at(user_id) + " 失败！");
            } catch (ApiError &) {
            }
        }
    }
}

MESSAGE_HANDLER(cqmoe_release, command("release") & contains("更新日志"), direct({SUPERUSER_ID})) {
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
            send_message(session.event.target, "格式不太对~");
        } catch (ApiError &) {
        }
        return;
    }

    auto update_info = std::string(first_space_it, session.event.message.cend());
    cq::utils::string_trim(update_info);
    try {
        send_group_message(RELEASE_GROUP_ID, update_info);
        send_message(session.event.target, "发布好啦！");
    } catch (ApiError &) {
    }
}
