cmake_minimum_required(VERSION 3.10)
project(Homework)

set(CMAKE_CXX_STANDARD 11)

find_package( OpenCV REQUIRED )

# include_directories(${OpenCV_INCLUDE_DIRS} /usr/include/eigen3)

add_executable(main main.cpp)
target_link_libraries(  main ${OpenCV_LIBS}  )