#include <iostream>

#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/highgui.hpp>


std::string width = "640";
std::string height = "480";

std::string host = "192.168.1.100";
std::string localhost = "127.0.0.1";
std::string port = "5000";

constexpr size_t fps = 20;
std::string framerate = std::to_string(fps) + "/1";


void sendVideo() {
    cv::VideoCapture cap(0);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    std::string pipeline = "appsrc ! videoconvert ! video/x-raw,format=YUY2,width=" + width +",height=" + height
                            + ",framerate=" + framerate + " ! jpegenc ! rtpjpegpay ! udpsink host=" + host + " port=" + port;

    cv::VideoWriter writer(pipeline, cv::CAP_GSTREAMER, 0, fps, cv::Size(640,480), true);

    if (!cap.isOpened() || !writer.isOpened()) {
        std::cout << "VideoCapture or VideoWriter not opened" << std::endl;
        exit(-1);
    }

    std::cout << "start sending" << std::endl;

    cv::Mat frame;

    while (cap.isOpened()) {
        cap.read(frame);

        if (frame.empty()) {
            continue;
        }

        writer.write(frame);
    }
}


int main() {
    sendVideo();
    return 0;
}
