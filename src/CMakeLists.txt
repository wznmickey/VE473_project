cmake_minimum_required(VERSION 3.10)
project(VE473P)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_PREFIX_PATH /opt/intel/openvino_2022/runtime/cmake/)
set(OURPATH /opt/intel/openvino_2022/runtime/lib/aarch64)
find_package( OpenCV REQUIRED )
find_package( ngraph REQUIRED)
find_package(InferenceEngine REQUIRED)
find_package( OpenVINO REQUIRED)
include_directories(/opt/intel/openvino_2022/runtime/include)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${ngraph_INCLUDE_DIRS})
include_directories(${InferenceEngine_INCLUDE_DIRS})
include_directories(${OpenVINO_INCLUDE_DIRS})
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -DEFF_DEBUG")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -DDEMO -DPERF_DEBUG")
add_executable(main Camera.cpp Detection.cpp distance.cpp Gyro.cpp buzzer.cpp Mode.cpp main.cpp)
target_link_libraries(  main  ${OURPATH}/libopenvino.so ${OpenCV_LIBS} MPU6050 wiringPi)

# add_executable(main_2 test_single.cpp)
# target_link_libraries(  main_2  ${OURPATH}/libopenvino.so ${OpenCV_LIBS} )

