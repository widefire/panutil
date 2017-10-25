# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

aux_source_directory(. DIR_THREAD_SRCS)
add_library(panutils_thread ${DIR_THREAD_SRCS})