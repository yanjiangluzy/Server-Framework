# 该目录下的所有源文件全部放进SRC_LIST
aux_source_directory (. SRC_LIST)

# 指明头文件的位置
include_directories (../include)

# 指明生成可执行程序的依赖
add_executable (main ${SRC_LIST})

# EXECUTABLE_OUTPUT_PATH ：目标二进制可执行文件的存放位置
# PROJECT_SOURCE_DIR：工程的根目录
# 该语句表示将目标二进制可执行文件的存放位置设置成工程根目录下的bin目录
set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
