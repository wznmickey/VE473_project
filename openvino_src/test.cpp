#include <inference_engine.hpp>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <sys/time.h>
#include <unistd.h>

using namespace InferenceEngine;

/**
 * @return float: time difference in ms
*/
float timeDiff(struct timeval start, struct timeval end) {
    float sdiff = (end.tv_sec - start.tv_sec) * 1000.0f;
    float usdiff = (end.tv_usec - start.tv_usec) / 1000.0f;
    return sdiff + usdiff;
}

static Blob::Ptr MywrapMat2Blob(const cv::Mat& mat) {
    size_t channels = mat.channels();
    size_t height = mat.size().height;
    size_t width = mat.size().width;

    size_t strideH = mat.step.buf[0];
    size_t strideW = mat.step.buf[1];

    bool is_dense = strideW == channels && strideH == channels * width;

    // OPENVINO_ASSERT(is_dense, "Doesn't support conversion from not dense cv::Mat");

    TensorDesc tDesc(Precision::FP32, {1, channels, height, width}, Layout::NHWC);

    return make_shared_blob<float>(tDesc, (float*)mat.data);
}


int main() {

    struct timeval startTime;
    struct timeval endTime;

    try {

        gettimeofday(&startTime, NULL);

        // load inference engine plugin
        InferenceEngine::Core ie;
        
        // Load Model
        std::string modelPath = "/home/pi/intel/vehicle-detection-0200/FP32/vehicle-detection-0200.xml";
        std::string modelBinPath = "/home/pi/intel/vehicle-detection-0200/FP32/vehicle-detection-0200.bin";
        CNNNetwork network = ie.ReadNetwork(modelPath, modelBinPath);
        
        // Get Input & Output Infomation
        InputsDataMap inputInfo(network.getInputsInfo());
        OutputsDataMap outputInfo(network.getOutputsInfo());
        
        // Prepare the Network
        std::string inputName;
        for (const auto &inputInfoItem : inputInfo) {
            inputName = inputInfoItem.first;
        }
        // Create the Network
        ExecutableNetwork executableNetwork = ie.LoadNetwork(network, "MYRIAD");
        InferRequest inferRequest = executableNetwork.CreateInferRequest();
        Blob::Ptr inputBlob = inferRequest.GetBlob(inputName);

        gettimeofday(&endTime, NULL);
        std::cout << "Load Model Time: " << timeDiff(startTime, endTime) << std::endl;

        // Load input image and resize
        cv::Mat image = cv::imread("car.jpg");
        std::cout << "image size at read: " << image.size() << std::endl;
        cv::resize(image, image, cv::Size(inputBlob->getTensorDesc().getDims()[3], inputBlob->getTensorDesc().getDims()[2]));
        cv::Mat resultImage = image.clone();
        // cv::cvtColor(image, image, cv::COLOR_RGB2BGR);
        // cv::transpose(image, image);
        auto input_data = inputBlob->buffer().as<PrecisionTrait<Precision::FP32>::value_type *>();
        for (size_t c = 0; c < 3; c++) {
            for (size_t h = 0; h < 256; h++) {
                for (size_t w = 0; w < 256; w++) {
                    input_data[c * 256 * 256 + h * 256 + w] = image.at<cv::Vec<float, 3>>(w, h)[c];
                }
            }
        }

        // Blob::Ptr imgBlob = MywrapMat2Blob(image.clone());
        //if (imgBlob->getTensorDesc().getPrecision() != Precision::FP32) {
        //    imgBlob->getTensorDesc().setPrecision(Precision::FP32);
        //}
        // inferRequest.SetBlob(inputName, imgBlob);
        // auto input_data = inputBlob->buffer().as<PrecisionTrait<Precision::FP32>::value_type *>();
        // ::copy(image.data, input_data);
        // std::cout << "image size: " << image.total() * image.channels() << std::endl;
        // std::copy(image.data, image.data + image.total() * image.channels(), input_data);

        // Network Inference
        gettimeofday(&startTime, NULL);
        inferRequest.Infer();
        gettimeofday(&endTime, NULL);
        std::cout << "Inference Time: " << timeDiff(startTime, endTime) << std::endl;

        // Get output
        std::string outputName;
        for (const auto &outputInfoItem : outputInfo) {
            outputName = outputInfoItem.first;
        }
        Blob::Ptr outputBlob = inferRequest.GetBlob(outputName);

        // Process output
        float *outputData = outputBlob->buffer().as<float *>();

        const auto outputDims = outputBlob->getTensorDesc().getDims();
        const int numClasses = outputDims[1]; // Number of classes
        const int outputHeight = outputDims[2]; // Grid height
        const int outputWidth = outputDims[3]; // Grid width

        // Calculate the number of detections
        const int numDetections = outputHeight;

        for (int i = 0; i < numDetections; ++i) {
            // Draw rectangle according to the info in output
            int x1 = int(outputData[i * 7 + 3] * image.cols);
            int y1 = int(outputData[i * 7 + 4] * image.rows);
            int x2 = int(outputData[i * 7 + 5] * image.cols);
            int y2 = int(outputData[i * 7 + 6] * image.rows);
            for (int j = 0; j < 7 ; j++)
                std::cout << outputData[i*7+j] << " ";
            std::cout << std::endl; 
            float confidence = (outputData[i*7 + 2]);
            std::cout << "conf: " << confidence << std::endl;
            if (confidence < 0.15) break;
            cv::rectangle(resultImage, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 255, 0), 2);
        }
        cv::imwrite("result_cpp.jpg", resultImage);


    } catch (const std::exception &error) {
        std::cerr << error.what() << std::endl;
        return 1;
    }
    
    return 0;
}
