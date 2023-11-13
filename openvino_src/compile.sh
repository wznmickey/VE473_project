g++ -o object_detection_test test.cpp \
-I /opt/intel/openvino_2022/runtime/include/ \
-I /opt/intel/openvino_2022/runtime/include/ie \
-I /usr/include/opencv4 \
-I /opt/intel/openvino_2022/samples/cpp/common/utils/include/ \
-I /opt/intel/openvino_2022/samples/cpp/common/format_reader/ \
-L/opt/intel/openvino_2022/runtime/lib/aarch64/ \
-lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lopencv_videoio -lopenvino
