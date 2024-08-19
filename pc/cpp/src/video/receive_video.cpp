#include <iostream>

#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/highgui.hpp>


std::string port = "5000";
std::string framerate = "20/1";


void receiveVideo() {
    std::string pipeline = "udpsrc port=" + port + " ! application/x-rtp,media=video,payload=26,clock-rate=90000,encoding-name=JPEG, \
    framerate=" + framerate + " ! rtpjpegdepay ! jpegdec ! videoconvert ! appsink";

    cv::VideoCapture cap(pipeline, cv::CAP_GSTREAMER);

    if (!cap.isOpened()) {
        std::cout <<"VideoCapture not opened" << std::endl;
        exit(-1);
    }

    std::cout << "start receiving" << std::endl;

    cv::Mat frame;

    while (cap.isOpened()) {
        cap.read(frame);

        if (frame.empty()) {
            continue;
        }
       
        cv::imshow("receiver", frame);
        if (cv::waitKey(1) == 'q') {
            break;
        } 
    }
    cv::destroyWindow("receiver");
}


int main() {
    receiveVideo();
    return 0;
}
