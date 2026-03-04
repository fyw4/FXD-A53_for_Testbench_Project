# CMakeLists.txt
cmake_minimum_required(VERSION 2.8)

########配置 ARM 交叉编译########
set(CMAKE_SYSTEM_NAME Linux)    #设置目标系统名字
set(CMAKE_SYSTEM_PROCESSOR arm) #设置目标处理器架构

# 指定编译器的 sysroot 路径
set(tools /opt/nxp-real-time-edge/2.1/sysroots)
set(CMAKE_SYSROOT ${tools}/cortexa53-crypto-poky-linux)

# 指定交叉编译器 arm-linux-gcc 和 arm-linux-g++
set(CMAKE_C_COMPILER ${tools}/x86_64-pokysdk-linux/usr/bin/aarch64-poky-linux/aarch64-poky-linux-gcc)
set(CMAKE_CXX_COMPILER ${tools}/x86_64-pokysdk-linux/usr/bin/aarch64-poky-linux/aarch64-poky-linux-g++)

# 为编译器添加编译选项
set(CMAKE_C_FLAGS "-std=c99 -Wall -O0 -lpthread -g3 -Wall -fmessage-length=0 -D_GNU_SOURCE")
set(CMAKE_CXX_FLAGS "-std=c++11 -DTEST -lpthread -Wall -O0 -g3 -Wall  -fmessage-length=0 -fPIC -D_GNU_SOURCE")

#从来不在指定目录(交叉编译)下查找工具程序。(编译时利用的是宿主的工具)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

#只在指定目录(交叉编译)下查找库文件
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)

#只在指定目录(交叉编译)下查找头文件
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

#只在指定的目录(交叉编译)下查找依赖包
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
#################################


########生成可执行文件###########
project(FXD-A53)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 程序所引用的头文件库目录。
include_directories(
        ${PROJECT_SOURCE_DIR}/include/
)

# 生成可执行文件
aux_source_directory(./src/ DIR_SRCS)
add_executable(FXD-A53 ${DIR_SRCS})

# 链接相关库
target_link_libraries(FXD-A53 PUBLIC pthread)
target_link_libraries(FXD-A53 PUBLIC rt)
target_link_libraries(FXD-A53 PUBLIC m)
target_link_libraries(FXD-A53 ${LIBRARY_OUTPUT_PATH}/libsocketcan.a)
#################################

message("\n #### FXD-A53 Makefile Created ! #### \n")

