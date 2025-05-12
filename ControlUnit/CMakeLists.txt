cmake_minimum_required(VERSION 3.10)
project(ControlUnit)

set(CMAKE_CXX_STANDARD 17)

find_package(PahoMqttCpp REQUIRED)
find_package(Threads REQUIRED)

add_executable(ControlUnit
    src/main.cpp
    src/MQTTManager.cpp
)

target_link_libraries(ControlUnit
    PahoMqttCpp::PahoMqttCpp
    ${CMAKE_THREAD_LIBS_INIT}
)