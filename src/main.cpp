#include <exception>
#include <format>
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <filesystem>

#include "opencv2/core/cvstd_wrapper.hpp"
#include "opencv2/core/hal/interface.h"
#include "opencv2/core/mat.hpp"
#include "opencv2/core/types.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/objdetect/face.hpp"

#include "algo/sort/sort.hpp"
#include "opencv2/videoio.hpp"

using namespace std;
using namespace cv;

static void visualize(Mat &input, int frame, Mat &faces, int thickness = 2) {
    if (frame >= 0)
        cout << "Frame " << frame << ", ";
    for (int i = 0; i < faces.rows; i++) {
        // Print results
        cout << "Face " << i << ", top-left coordinates: ("
             << faces.at<float>(i, 0) << ", " << faces.at<float>(i, 1) << "), "
             << "box width: " << faces.at<float>(i, 2)
             << ", box height: " << faces.at<float>(i, 3) << ", "
             << "score: " << cv::format("%.2f", faces.at<float>(i, 14)) << endl;

        // Draw bounding box
        rectangle(input,
                  Rect2i(int(faces.at<float>(i, 0)), int(faces.at<float>(i, 1)),
                         int(faces.at<float>(i, 2)),
                         int(faces.at<float>(i, 3))),
                  Scalar(0, 255, 0), thickness);
        // Draw landmarks
        circle(input,
               Point2i(int(faces.at<float>(i, 4)), int(faces.at<float>(i, 5))),
               2, Scalar(255, 0, 0), thickness);
        circle(input,
               Point2i(int(faces.at<float>(i, 6)), int(faces.at<float>(i, 7))),
               2, Scalar(0, 0, 255), thickness);
        circle(input,
               Point2i(int(faces.at<float>(i, 8)), int(faces.at<float>(i, 9))),
               2, Scalar(0, 255, 0), thickness);
        circle(
            input,
            Point2i(int(faces.at<float>(i, 10)), int(faces.at<float>(i, 11))),
            2, Scalar(255, 0, 255), thickness);
        circle(
            input,
            Point2i(int(faces.at<float>(i, 12)), int(faces.at<float>(i, 13))),
            2, Scalar(0, 255, 255), thickness);
    }
}

std::string faceDetOnnxPath("./dnn/face_detection_yunet_2022mar.onnx");
std::string faceRecOnnxPath("./dnn/face_recognition_sface_2021dec.onnx");

auto getFeature(std::string const & imgPath) -> cv::Mat {
    cv::Mat mm(cv::imread(imgPath));

    cv::resize(mm, mm,
                cv::Size(200, 300));

    auto faceDtor{
        cv::FaceDetectorYN::create(faceDetOnnxPath, "", mm.size())};

    cv::Mat mmDetected;

    faceDtor->detect(mm, mmDetected);

    auto faceRec {cv::FaceRecognizerSF::create(faceRecOnnxPath, "")};

    cv::Mat alignedFace;

    faceRec->alignCrop(mm, mmDetected.row(0), alignedFace);

    cv::Mat featured;

    faceRec->feature(alignedFace, featured);

    return featured;
}

auto main() -> int {
    try {
        auto faceRec {cv::FaceRecognizerSF::create(faceRecOnnxPath, "")};

        double cosScore {faceRec->match(getFeature("assets/me.jpg"), getFeature("assets/me2.jpg"))};

        if (cosScore >= 0.363) {
            std::cout << "same person!\n";
        } else {
            std::cout << "not same person!\n";
        }

        // cv::VideoCapture cap(0);

        // while (true) {
        //     cap.read(mm);

        //     cv::resize(mm, mm, cv::Size(200, 300));
        //     faceDtor->detect(mm, mmDetected);

        //     visualize(mm, -1, mmDetected);

        //     cv::imshow("mm", mm);

        //     cv::waitKey(50);
        // }
    } catch (std::exception const &e) {
        std::cerr << e.what() << '\n';
    }

    return 0;
}