#include "car_detection.h"

vector<string> classes;

float MAX_DISTANCE_MARGIN = 250;

Car_Detection::Car_Detection()
{
    // Load names of classes
    string classesFile = "model/car-obj.names";
    ifstream ifs(classesFile.c_str());
    string line;
    while (getline(ifs, line)) classes.push_back(line);
    
    // Give the configuration and weight files for the model
    String modelConfiguration = "model/yolov3-tiny-car.cfg";
    String modelWeights = "model/yolov3-tiny-car.weights";

    // Load the network
    net = readNetFromDarknet(modelConfiguration, modelWeights);
    net.setPreferableBackend(DNN_BACKEND_OPENCV);
    net.setPreferableTarget(DNN_TARGET_CPU);
}


/**
 * @brief postprocess the network output, find the car block
 * @return TODO: to be determined
*/
vector<cv::Rect2d> Car_Detection::findCars(Mat& frame, const vector<Mat>& outs, float confThreshold) {
    vector<int> classIds;
    vector<float> confidences;
    vector<Rect> boxes;
    vector<Point> centers;  
    vector<cv::Rect2d> res = vector<cv::Rect2d>();  

    for (size_t i = 0; i < outs.size(); ++i) {
        // Scan through all the bounding boxes output from the network and keep only the
        // ones with high confidence scores. Assign the box's class label as the class
        // with the highest score for the box.
        float* data = (float*)outs[i].data;
        for (int j = 0; j < outs[i].rows; ++j, data += outs[i].cols)
        {
            Mat scores = outs[i].row(j).colRange(5, outs[i].cols);
            Point classIdPoint;
            double confidence;
            // Get the value and location of the maximum score
            minMaxLoc(scores, 0, &confidence, 0, &classIdPoint);
            if (confidence > confThreshold) {
                int centerX = (int)(data[0] * frame.cols);
                int centerY = (int)(data[1] * frame.rows);
                int width = (int)(data[2] * frame.cols);
                int height = (int)(data[3] * frame.rows);
                int left = centerX - width / 2;
                int top = centerY - height / 2;
                
                classIds.push_back(classIdPoint.x);
                confidences.push_back((float)confidence);
                boxes.push_back(Rect(left, top, width, height));

                std::cout << "car detected at:";
                std::cout << "left: " << left << ", top: " << top
                    << ", width: " << width << ", height: " << height << endl;
                res.push_back(cv::Rect2d(left,top,width,height));
            }
        }
    }
    return res;
}

/**
 * @return TODO: to be determined
*/
vector<cv::Rect2d> Car_Detection::callNetworks(Mat & frame) {
    // Initialize the parameters
    int inpWidth = 416;  // Width of network's input image
    int inpHeight = 416; // Height of network's input image


    // Create a 4D blob from a frame.
    Mat blob;
    blobFromImage(frame, blob, 1/255.0, cv::Size(inpWidth, inpHeight), Scalar(0,0,0), true, false);
    
    //Sets the input to the network
    net.setInput(blob);
    
    // Runs the forward pass to get output of the output layers
    vector<Mat> outs;
    net.forward(outs, getOutputsNames(net));
    
    //return findCars(frame, outs);
    return findCars(frame, outs);
}

/**
 * @brief Get the names of the output layers
 */
vector<String> Car_Detection::getOutputsNames(const Net& net)
{
    static vector<String> names;
    if (names.empty())
    {
        //Get the indices of the output layers, i.e. the layers with unconnected outputs
        vector<int> outLayers = net.getUnconnectedOutLayers();
        
        //get the names of all the layers in the network
        vector<String> layersNames = net.getLayerNames();
        
        // Get the names of the output layers in names
        names.resize(outLayers.size());
        for (size_t i = 0; i < outLayers.size(); ++i)
        names[i] = layersNames[outLayers[i] - 1];
    }
    return names;
}