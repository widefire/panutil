# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

aux_source_directory(. DIR_STRUCTS_SRCS)

file(GLOB_RECURSE CURRENT_HEADERS  *.h *.hpp)
source_group("Include" FILES ${CURRENT_HEADERS})

#添加目录 
#add_subdirectory(../thread)
#add_subdirectory(../structs)
include_directories(../structs)

MESSAGE(${PROJECT_BINARY_DIR})
link_libraries(${PROJECT_BINARY_DIR}/panUtils/structs/panutils_structs)


add_library(panutils_network ${DIR_STRUCTS_SRCS} ${CURRENT_HEADERS})