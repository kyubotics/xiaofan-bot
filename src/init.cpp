#include <cqcppsdk/cqcppsdk.h>

#include "app.hpp"
#include "handler.hpp"

CQ_INIT {
    using namespace cq;
    using namespace cq::message;
    using namespace xiaofan;

    on_coolq_start(startup);
    on_enable(startup);
    on_disable(shutdown);
    on_coolq_exit(shutdown);

    on_message(run_handlers<MessageEvent>);
    on_notice(run_handlers<NoticeEvent>);
    on_request(run_handlers<RequestEvent>);
}
