#include <iostream>

#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/highgui.hpp>


size_t width = 640;
size_t height = 480;

std::string host = "192.168.1.100";
size_t port = 554;

size_t fps = 30;
std::string framerate = std::to_string(fps) + "/1";


void sendVideo() {
    auto cap = cv::VideoCapture(4);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);

    std::string pipeline = "appsrc ! videoconvert ! video/x-raw,format=YUY2,width=" + std::to_string(width)
                            + ",height=" + std::to_string(height) + ",framerate=" + framerate 
                            + "! jpegenc ! rtpjpegpay ! udpsink host=" + host + "port=" + std::to_string(port);


    cv::VideoWriter writer(pipeline, cv::CAP_GSTREAMER, 0, 30, cv::Size(width, height), true);

    if (!cap.isOpened() || !writer.isOpened()) {
        std::cout << "VideoCapture or VideoWriter not opened" << std::endl;
        exit(-1);
    }

    cv::Mat frame;

    while (cap.isOpened()) {
        cap.read(frame);

        if (frame.empty()) {
            break;
        }

        writer.write(frame);
    }
}


int main() {
    sendVideo();
    return 0;
}
