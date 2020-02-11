#include <dolores/dolores.hpp>

CQ_INIT {
    dolores::init();
    dolores::on_startup([] { cq::logging::info("app", "开工！"); });
    dolores::on_shutdown([] { cq::logging::info("app", "下班！"); });
}
