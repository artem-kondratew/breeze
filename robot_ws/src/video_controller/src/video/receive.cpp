#include <iostream>

#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/highgui.hpp>


void receiver()
{    
    // The sink caps for the 'rtpjpegdepay' need to match the src caps of the 'rtpjpegpay' of the sender pipeline
    // Added 'videoconvert' at the end to convert the images into proper format for appsink, without
    // 'videoconvert' the receiver will not read the frames, even though 'videoconvert' is not present
    // in the original working pipeline
    cv::VideoCapture cap("udpsrc port=5000 ! application/x-rtp,media=video,payload=26,clock-rate=90000,encoding-name=JPEG,framerate=30/1 ! rtpjpegdepay ! jpegdec ! videoconvert ! appsink",cv::CAP_GSTREAMER);

    if(!cap.isOpened())
    {
        std::cout<<"VideoCapture not opened"<<std::endl;
        exit(-1);
    }

    cv::Mat frame;

    while(true) {

        cap.read(frame);

        if(frame.empty())
            break;

        imshow("Receiver", frame);
        if(cv::waitKey(1) == 'r')
            break;
    }
    cv::destroyWindow("Receiver");
}


int main() {
    receiver();
}