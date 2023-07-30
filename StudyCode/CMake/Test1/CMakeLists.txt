cmake_minimum_required (VERSION 2.8)

project (demo)


# 1. 通过aux_source_directory将指定目录下的所有源文件全部添加到变量SRC_LIST中
# 2. 通过set添加指定的源文件到SRC_LIST中
# aux_source_directory(CMake SRC_LIST)

set( SRC_LIST 
     ../src/TestCMake.cc)   

add_executable(TestCMake ${SRC_LIST})
