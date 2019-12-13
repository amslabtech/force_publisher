cmake_minimum_required(VERSION 2.8)
project(force_publisher)

set(CMAKE_CXX_FLAGS "-O2 -std=c++11 -Wall")
set(MOSQUITTO_DIR "/home/pi/dev/MqttClass")

add_executable(force_publisher src/force_publisher.cpp)

target_include_directories(force_publisher   
	PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
	PUBLIC $<BUILD_INTERFACE:${MOSQUITTO_DIR}/include>
)

target_link_libraries(force_publisher mosquitto RTIMULib phidget22)

INSTALL(
	TARGETS force_publisher
	RUNTIME DESTINATION /usr/local/bin/
)

INSTALL(
	FILES include/force_structure.hpp
	DESTINATION /usr/local/include/
)

