#pragma once

#include <iostream>
#include <fstream>

#include "opencv2/core/cvstd_wrapper.hpp"
#include "opencv2/core/mat.hpp"
#include "opencv2/core/types.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/objdetect/face.hpp"

namespace gd__ {
    struct dnnWorkConf {
        static constexpr
        char const *faceDetOnnxPath{"./dnn/face_detection_yunet_2022mar.onnx"};

        static constexpr
        char const *faceRecOnnxPath{"./dnn/face_recognition_sface_2021dec.onnx"};

        static
        auto visualize(cv::Mat &input, cv::Mat &faces, int thickness = 2) -> void {
            using namespace std;
            using namespace cv;

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

        static
        auto getFeature(cv::Ptr<cv::FaceDetectorYN> faceDtor, cv::Ptr<cv::FaceRecognizerSF> faceRec,
                        cv::Mat &imgMat) -> cv::Mat {
            cv::Mat imgDetected;

            faceDtor->detect(imgMat, imgDetected);

            cv::Mat imgAligned;

            faceRec->alignCrop(imgMat, imgDetected.row(0), imgAligned);

            cv::Mat featured;

            faceRec->feature(imgAligned, featured);

            return featured;
        }

        //we recommand the value is 0.363
        static
        auto isSameEntity(cv::Ptr<cv::FaceRecognizerSF> faceRec, cv::Mat const &feature1,
                          cv::Mat const &feature2) -> bool {
            return 0.20 < faceRec->match(feature1, feature2);
        }

//! Write cv::Mat as binary
/*!
\param[out] ofs output file stream
\param[in] out_mat mat to save
*/
        static
        bool writeMatBinary(std::ofstream &ofs, const cv::Mat &out_mat) {
            if (!ofs.is_open()) {
                return false;
            }
            if (out_mat.empty()) {
                int s = 0;
                ofs.write((const char *) (&s), sizeof(int));
                return true;
            }
            int type = out_mat.type();
            ofs.write((const char *) (&out_mat.rows), sizeof(int));
            ofs.write((const char *) (&out_mat.cols), sizeof(int));
            ofs.write((const char *) (&type), sizeof(int));
            ofs.write((const char *) (out_mat.data), out_mat.elemSize() * out_mat.total());

            return true;
        }


//! Save cv::Mat as binary
/*!
\param[in] filename filaname to save
\param[in] output cvmat to save
*/
        static
        bool SaveMatBinary(const std::string &filename, const cv::Mat &output) {
            std::ofstream ofs(filename, std::ios::binary);
            return writeMatBinary(ofs, output);
        }


//! Read cv::Mat from binary
/*!
\param[in] ifs input file stream
\param[out] in_mat mat to load
*/
        static
        bool readMatBinary(std::ifstream &ifs, cv::Mat &in_mat) {
            if (!ifs.is_open()) {
                return false;
            }

            int rows, cols, type;
            ifs.read((char *) (&rows), sizeof(int));
            if (rows == 0) {
                return true;
            }
            ifs.read((char *) (&cols), sizeof(int));
            ifs.read((char *) (&type), sizeof(int));

            in_mat.release();
            in_mat.create(rows, cols, type);
            ifs.read((char *) (in_mat.data), in_mat.elemSize() * in_mat.total());

            return true;
        }


//! Load cv::Mat as binary
/*!
\param[in] filename filaname to load
\param[out] output loaded cv::Mat
*/
        static
        bool LoadMatBinary(const std::string &filename, cv::Mat &output) {
            std::ifstream ifs(filename, std::ios::binary);
            return readMatBinary(ifs, output);
        }
    };
}