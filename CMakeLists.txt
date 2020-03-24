cmake_minimum_required(VERSION 3.9)
project(xiaofan)

set(CMAKE_CXX_STANDARD 17)

include(extern/cqcppsdk/cqcppsdk.cmake)

include_directories(src)

file(READ "app_id.txt" APP_ID)
string(STRIP "${APP_ID}" APP_ID)
cq_set_app_id(${APP_ID})

file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

if (CQ_CAN_BUILD_STD_MODE)
    cq_add_std_app(${SOURCE_FILES})
    cq_install_std_app()
endif ()

cq_add_dev_app(${SOURCE_FILES})
