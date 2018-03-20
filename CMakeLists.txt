# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

# 项目信息
project (panutil)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set (PROJECTNAME_ARCHITECTURE "x64")
# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(. DIR_SRCS)

#添加包含目录
#INCLUDE_DIRECTORIES(panUtils/thread panUtils/structs)

#添加目录 
add_subdirectory(panUtils/thread)
add_subdirectory(panUtils/structs)
add_subdirectory(panUtils/network)
add_subdirectory(panUtils/log4p)
add_subdirectory(panUtils/baseUtils)

file(GLOB_RECURSE CURRENT_HEADERS  *.h *.hpp)
source_group("Include" FILES ${CURRENT_HEADERS})
#指定生成目标
add_executable(Demo ${DIR_SRCS} ${CURRENT_HEADERS})

#添加链接库
IF (CMAKE_SYSTEM_NAME MATCHES "Linux") 
TARGET_LINK_LIBRARIES(Demo  pthread panutils_thread panutils_structs panutils_network panutils_log4p panutils_utils)
else()
TARGET_LINK_LIBRARIES(Demo  panutils_thread panutils_structs panutils_network panutils_log4p panutils_utils)
ENDIF ()  