from time import time

start = time()

import cv2
import numpy as np
from openvino.inference_engine import IECore

print("import cv time: " + str(time()-start))

start = time()

# Load Model
model_xml = "/home/pi/intel/vehicle-detection-0200/FP16/vehicle-detection-0200.xml"
model_bin = "/home/pi/intel/vehicle-detection-0200/FP16/vehicle-detection-0200.bin"

ie = IECore()
net = ie.read_network(model=model_xml, weights=model_bin)

# Load model to device
exec_net = ie.load_network(network=net, device_name="MYRIAD")

# Load input
input_blob = next(iter(net.input_info))
output_blob = next(iter(net.outputs))

end = time()
print("load model time: "+str(end-start))


startfor = time()
for i in range(1):

    image = cv2.imread("car.jpg")

    image = cv2.resize(image, (256, 256))  # resize
    output_image = image.copy()
    print(image.shape)
    # image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)
    image = image.transpose((2, 0, 1))  # (3, 256, 256)
    # image = image.transpose((1, 2, 0))  # RGB channel order

    print(image[0][0])
    print(image.shape)

    end = time()
    print("read and resize time: "+str(end-start))

    start = time()
    # Inference
    result = exec_net.infer(inputs={input_blob: [image]})
    print(image[0][0])
    print(image.shape)
    end = time()
    print("inference time: "+str(end-start))

    # Process output
    start = time()
    output = result[output_blob]
    detections = output[0][0]

    for detection in detections:
        confidence = detection[2]
        if confidence >= 0.1:  # Threshold here
            print(detection)
            x_min, y_min, x_max, y_max = detection[3:7] * np.array([image.shape[2], image.shape[1], 
                                                                    image.shape[2], image.shape[1]])
            x_min, y_min, x_max, y_max = map(int, [x_min, y_min, x_max, y_max])
            print(x_min, y_min, x_max, y_max)

            # cv2.line(img=output_image, pt1=(x_min, y_min), pt2=(x_max, y_max), color=(0,0,255))
            cv2.rectangle(img=output_image, pt1=(x_min, y_min), pt2=(x_max, y_max), color=(0,0,255))  # Plot Barbox

    print("process time: "+str(time()-start))

endfor = time()
print("process*10 time: "+str(endfor-startfor))

# Show result
cv2.imwrite("result.jpg", output_image)

