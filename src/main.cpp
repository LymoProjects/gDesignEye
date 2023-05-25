#include <exception>
#include <fstream>
#include <iostream>

#include <co/co.h>

#include "opencv2/core/cvstd_wrapper.hpp"
#include "opencv2/objdetect/face.hpp"
#include "opencv2/core/mat.hpp"
#include "opencv2/core/types.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"

#include "pref.hpp"
#include "dnnWorkConf.hpp"
#include "featureLoader.hpp"

auto main() -> int {
    if (!gd__::pref::init()) {
        std::cerr << "pref init failed!\n";

        return -1;
    }

    cv::VideoCapture cap;
    cv::Ptr<cv::FaceDetectorYN> faceDtor;

    try {
        cap.open(gd__::pref::captureIndex);

        faceDtor = cv::FaceDetectorYN::create(gd__::dnnWorkConf::faceDetOnnxPath, "", cv::Size(200, 300));
    } catch (std::exception const & e) {
        std::cerr << e.what() << '\n';

        return -1;
    }

    cv::Mat img;
    cv::Mat detected;

    while (true) {
        try {
            cap.read(img);

            //cv::imshow("capture " + std::to_string(gd__::pref::captureIndex), img);

            cv::resize(img, img, cv::Size(200, 300));

            faceDtor->detect(img, detected);

            cv::Mat copyImg(img);
            gd__::dnnWorkConf::visualize(copyImg, detected);
            cv::imshow("visualize capture " + std::to_string(gd__::pref::captureIndex), copyImg);

            // if (detected.rows >= 1 && 0.95 < detected.at<float>(0, 14)) {
            //     go([](cv::Mat img) {
            //         gd__::featureLoader::ref().identify(img);
            //     }, img);
            // }

            cv::waitKey(100);
        } catch (std::exception const & e) {
            std::cerr << e.what() << '\n';
        }
    }

    return 0;
}