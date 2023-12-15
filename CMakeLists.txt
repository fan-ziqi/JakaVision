cmake_minimum_required(VERSION 3.22)
project(JakaVision)

set(CMAKE_CXX_STANDARD 14)

link_libraries(ws2_32)

add_executable(JakaVision
        main.cpp
        OpenCVFuncs.cpp
        JAKARobot_API.cpp
        jakauser.cpp)

# 设置OpenCV目录
set(OpenCV_DIR E:/Install/Tools/opencv/mingw_build/install)
# 搜索OpenCV目录
find_package(OpenCV REQUIRED)
## 添加OpenCV头文件目录
#include_directories(${OpenCV_INCLUDE_DIRS})
# 链接OpenCV库文件
target_link_libraries(JakaVision ${OpenCV_LIBS})
