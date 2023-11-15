#include <cstddef>
#include <iostream>
#include <opencv2/core/types.hpp>
#include <opencv2/opencv.hpp>
#include <openvino/openvino.hpp>
#include <sys/time.h>
#include <unistd.h>

/**
 * @return float: time difference in ms
 */
float timeDiff( struct timeval start, struct timeval end )
{
    float sdiff  = ( end.tv_sec - start.tv_sec ) * 1000.0f;
    float usdiff = ( end.tv_usec - start.tv_usec ) / 1000.0f;
    return sdiff + usdiff;
}

class DetectionModel
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

    void imageConvert( cv::Mat &input )
    {
        cv::Mat blob_image;
        cv::resize( input, blob_image, cv::Size( width, height ) );
        float *image_data = input_tensor.data< float >( );
        for ( size_t c = 0; c < channel; c++ )
        {
            for ( size_t h = 0; h < height; h++ )
            {
                for ( size_t w = 0; w < width; w++ )
                {
                    size_t index         = c * width * height + h * width + w;
                    image_data [ index ] = blob_image.at< cv::Vec3b >( h, w ) [ c ];
                }
            }
        }
    }

public:
    DetectionModel( )
    {
        gettimeofday( &startTime, nullptr );
        std::string modelPath = "/home/pi/intel/vehicle-detection-0200/FP32/vehicle-detection-0200.xml";
        core;
        compiled_model = core.compile_model( modelPath, "MYRIAD" );
        infer_request  = compiled_model.create_infer_request( );

        gettimeofday( &endTime, nullptr );
        std::cout << "Load Model Time: " << timeDiff( startTime, endTime ) << std::endl;

        input_tensor           = infer_request.get_input_tensor( 0 );
        ov::Shape tensor_shape = input_tensor.get_shape( );
        channel                = tensor_shape [ 1 ];
        height                 = tensor_shape [ 2 ];
        width                  = tensor_shape [ 3 ];
        return;
    }
    std::vector< cv::Rect2i > detect( cv::Mat &img )
    {
        cv::Mat resultImage = img.clone( );

        imageConvert( img );
        gettimeofday( &startTime, NULL );

        infer_request.infer( );
        gettimeofday( &endTime, NULL );
        std::cout << "Inference Time: " << timeDiff( startTime, endTime ) << std::endl;
        auto   output1    = infer_request.get_output_tensor( 0 );
        float *outputData = output1.data< float >( );

        const auto outputDims   = output1.get_shape( );
        const int  outputHeight = outputDims [ 2 ]; // Grid height

        // Calculate the number of detections
        const int                 numDetections = outputHeight;
        std::vector< cv::Rect2i > ret;
        for ( int i = 0; i < numDetections; ++i )
        {
            float confidence = ( outputData [ i * 7 + 2 ] );
            if ( confidence < 0.15 )
                continue;
            std::cout << "conf: " << confidence << std::endl;

            // Draw rectangle according to the info in output
            int x1 = int( outputData [ i * 7 + 3 ] * img.cols );
            int y1 = int( outputData [ i * 7 + 4 ] * img.rows );
            int x2 = int( outputData [ i * 7 + 5 ] * img.cols );
            int y2 = int( outputData [ i * 7 + 6 ] * img.rows );
            ret.emplace_back( x1, y1, x2 - x1, y2 - y1 );
            for ( int j = 0; j < 7; j++ )
                std::cout << outputData [ i * 7 + j ] << " ";
            std::cout << std::endl;
            cv::rectangle( resultImage, cv::Point( x1, y1 ), cv::Point( x2, y2 ), cv::Scalar( 0, 255, 0 ), 2 );
        }
        cv::imwrite( "result_cpp.jpg", resultImage );

        return ret;
    }
};

int main( )
{
    struct timeval startTime;
    struct timeval endTime;

    gettimeofday( &startTime, nullptr );

    try
    {
        // Load Model
        DetectionModel detectionModel;
        for ( int i = 0; i < 100; i++ )
        {
            cv::Mat src = cv::imread( "../car.jpg", 1 );
            detectionModel.detect( src );
        }
    }
    catch ( const std::exception &error )
    {
        std::cerr << error.what( ) << std::endl;
        return 1;
    }
    gettimeofday( &endTime, nullptr );
    std::cout << "Load Model Time: " << timeDiff( startTime, endTime ) << std::endl;

    return 0;
}
