cmake_minimum_required(VERSION 3.13.1)

include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(zephyr-sample)

file(GLOB app_sources src/*.c src/*.cpp)

include_directories(src)

target_sources(app PRIVATE ${app_sources} ${smp_srv_sources})
