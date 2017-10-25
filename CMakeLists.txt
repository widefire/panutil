# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

# 项目信息
project (pantuil)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(. DIR_SRCS)

#添加包含目录
#INCLUDE_DIRECTORIES(panUtils/thread panUtils/structs)

#添加目录 
add_subdirectory(panUtils/thread)
add_subdirectory(panUtils/structs)


#指定生成目标
add_executable(Demo ${DIR_SRCS})

TARGET_LINK_LIBRARIES(Demo panutils_thread panutils_structs)