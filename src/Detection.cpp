#include "Detection.h"

/**
 * @return float: time difference in ms
 */
float timeDiff( struct timeval start, struct timeval end )
{
    float sdiff  = ( end.tv_sec - start.tv_sec ) * 1000.0f;
    float usdiff = ( end.tv_usec - start.tv_usec ) / 1000.0f;
    return sdiff + usdiff;
}

void Detection::imageConvert( cv::Mat &input )
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

void Detection::drawRectText( cv::Rect2i roi, std::string text)
{
    cv::rectangle(this->img, roi, cv::Scalar(0, 0, 255), 2);
    cv::Point position(roi.x+10,roi.y+roi.height+20);
    int fontFace = cv::FONT_HERSHEY_SIMPLEX;
    double fontScale = 0.8;
    cv::Scalar fontColor(255, 0, 0);
    int thickness = 2;
    cv::putText(this->img, text, position, fontFace, fontScale, fontColor, thickness);
    return;
}

void Detection::ImgSave(std::string savepath)
{
    cv::imwrite(savepath,this->img);
    return;
}

Detection::Detection( )
{
    gettimeofday( &startTime, nullptr );
    std::string modelPath = "/home/pi/intel/vehicle-detection-0200/FP16/vehicle-detection-0200.xml";
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
std::vector< cv::Rect2i >& Detection::get( )
{
    ret.clear();
    infer_request.wait( );
    gettimeofday(&endTime, NULL);
    std::cout << "Async process time: " << timeDiff(startTime, endTime) << std::endl;

    gettimeofday(&startTime, NULL);
    auto       output1      = infer_request.get_output_tensor( 0 );
    float     *outputData   = output1.data< float >( );
    const auto outputDims   = output1.get_shape( );
    const int  numDetections = outputDims [ 2 ];

    int i;
    for ( i = 0; i < numDetections; ++i )
    {
        float confidence = ( outputData [ i * 7 + 2 ] );
        if ( confidence < 0.3 ) break;
        // std::cout << "conf: " << confidence << std::endl;
        // Draw rectangle according to the info in output
        int x1 = int( outputData [ i * 7 + 3 ] * cols );
        int y1 = int( outputData [ i * 7 + 4 ] * rows );
        int x2 = int( outputData [ i * 7 + 5 ] * cols );
        int y2 = int( outputData [ i * 7 + 6 ] * rows );
        ret.emplace_back( x1, y1, x2 - x1, y2 - y1 );
        // drawRectText(cv::Rect2i(x1, y1, x2 - x1, y2 - y1));
        // for ( int j = 0; j < 7; j++ )
        //     std::cout << outputData [ i * 7 + j ] << " ";
    }
    // ImgSave("/home/pi/VE473_project/img/result.png");
    gettimeofday(&endTime, NULL);
    std::cout << "Get inference output: " << timeDiff(startTime, endTime) << ", with " << i <<
        " numbers of outputs.\n";
    return ret;
}

void Detection::detect( cv::Mat &img )
{

    this->img = img.clone();
    cols = img.cols;
    rows = img.rows;
    gettimeofday(&startTime, NULL);
    imageConvert( img );

    infer_request.start_async( );
}


// int main( )
// {
//     struct timeval startTime;
//     struct timeval endTime;

//     gettimeofday( &startTime, nullptr );

//     try
//     {
//         // Load Model
//         DetectionModel detectionModel;
//         for ( int i = 0; i < 100; i++ )
//         {
//             cv::Mat src = cv::imread( "../car.jpg", 1 );
//             detectionModel.detect( src );
//             detectionModel.get( );
//         }
//     }
//     catch ( const std::exception &error )
//     {
//         std::cerr << error.what( ) << std::endl;
//         return 1;
//     }
//     gettimeofday( &endTime, nullptr );
//     std::cout << "Load Model Time: " << timeDiff( startTime, endTime ) << std::endl;

//     return 0;
// }
