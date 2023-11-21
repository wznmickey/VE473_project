#ifndef DETECTION_H
#define DETECTION_H

#include <cstddef>
#include <iostream>
#include <opencv2/core/types.hpp>
#include <opencv2/opencv.hpp>
#include <openvino/openvino.hpp>
#include <sys/time.h>
#include <unistd.h>

float timeDiff( struct timeval start, struct timeval end );

class Detection
{

private:
    size_t            channel;
    size_t            height;
    size_t            width;
    ov::Core          core;
    ov::CompiledModel compiled_model;
    ov::InferRequest  infer_request;
    ov::Tensor        input_tensor;
    struct timeval    startTime;
    struct timeval    endTime;
    size_t            cols;
    size_t            rows;
    cv::Mat           img;
    void              imageConvert( cv::Mat &input );

public:
    Detection( );
    std::vector< cv::Rect2i > get( );
    void detect( cv::Mat &img );
    void              drawRectText( cv::Rect2i roi, std::string text);
    void              ImgSave(std::string savepath);
    cv::Mat&          getImage();
};

#endif