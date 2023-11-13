# g++ -ggdb car_tracking_yolo.cpp car.cpp -o car_tracking_yolo `pkg-config --cflags --libs opencv`

g++ -I /usr/include/opencv4 -o test src/*.cpp \
    -lopencv_core -lopencv_dnn -lopencv_imgproc -lopencv_videoio -lopencv_imgcodecs -lopencv_highgui -lopencv_calib3d
