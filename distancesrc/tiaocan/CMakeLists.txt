cmake_minimum_required(VERSION 3.10)
project(Homework)

set(CMAKE_CXX_STANDARD 20)

find_package( OpenCV REQUIRED )

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(/usr/include/matplotlib-cpp)
include_directories(/home/wznmickey/.pyenv/versions/3.8.17/lib/python3.8/site-packages/numpy/core/include)
include_directories(/home/wznmickey/.pyenv/versions/3.8.17/include/python3.8)

add_executable(main main.cpp)
target_link_libraries(  main ${OpenCV_LIBS} /home/wznmickey/.pyenv/versions/3.8.17/lib/libpython3.8.so   )
