cmake_minimum_required(VERSION 3.9)
project(xiaofan)

set(CMAKE_CXX_STANDARD 17)

include_directories(src)
include_directories(extern/cqcppsdk/include)

include(extern/cqcppsdk/cqcppsdk.cmake)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp)

message(STATUS ${SOURCE_FILES})

file(READ "app_id.txt" APP_ID)
string(STRIP "${APP_ID}" APP_ID)

cq_set_app_id(${APP_ID})

set(LIB_NAME "app")

if(MSVC AND (CMAKE_SIZEOF_VOID_P EQUAL 4)) # MSVC x86
    cq_add_app(${LIB_NAME} ${SOURCE_FILES}) # 添加 std 模式的动态链接库构建目标
    cq_add_install_script(${LIB_NAME} "${PROJECT_SOURCE_DIR}/scripts/install.ps1")
endif()

# 添加 dev 模式的可执行文件构建目标
set(CQCPPSDK_DEV_MODE ON)
cq_add_app(${LIB_NAME}_dev ${SOURCE_FILES})
