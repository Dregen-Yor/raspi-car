cmake_minimum_required(VERSION 3.1)
project(raspi-car)
set(CMAKE_CXX_STANDARD 17)
set(SOURCE_FILES main.cpp handler.h)

# 查找 oatpp 依赖
find_package(oatpp REQUIRED)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
find_library(WIRINGPI_LIBRARIES NAMES wiringPi)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
# 将目标文件与库文件进行链接
target_link_libraries(${PROJECT_NAME} oatpp::oatpp)
target_link_libraries( ${PROJECT_NAME} ${OpenCV_LIBS} )
target_link_libraries(${PROJECT_NAME} ${WIRINGPI_LIBRARIES})
