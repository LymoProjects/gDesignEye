#pragma once

#include <map>
#include <string>
#include <filesystem>
#include <iostream>

#include <co/json.h>
#include <co/time.h>
#include <co/http.h>
#include <co/log.h>
#include <co/def.h>
#include <co/co/mutex.h>

#include "opencv2/core/cvstd_wrapper.hpp"
#include "opencv2/core/mat.hpp"
#include "opencv2/core/types.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/objdetect/face.hpp"

#include "pref.hpp"
#include "httpHeaderConf.hpp"
#include "dnnWorkConf.hpp"

namespace gd__ {
    class featureLoader {
        std::map<std::string, cv::Mat> features;

        std::string lastLogInsertUser;
        int64 lastLogInsertTime {};

        co::Mutex mut;

        static inline
        co::Mutex outputMut;

        auto loadFromLocal() -> void {
            for (auto const & fsFile : std::filesystem::directory_iterator("features/")) {
                cv::Mat featureFromBin;

                std::string fileFullName(fsFile.path().string());
                std::string fileName(fsFile.path().filename().string());
                std::string filePureName(fileName.substr(0, fileName.rfind('.')));

                dnnWorkConf::LoadMatBinary(fileFullName, featureFromBin);

                features.insert(std::make_pair(filePureName, featureFromBin));
            }
        }

        auto loadFromServer() -> void {
            // todo.
        }

        explicit featureLoader() {
            loadFromLocal();
        }
    public:
        featureLoader(featureLoader const &) = delete;
        auto operator=(featureLoader const &) -> featureLoader & = delete;

        static
        auto ref() -> featureLoader & {
            featureLoader static fl_;

            return fl_;
        }

        auto identify(cv::Mat & img) -> void {
            auto faceDtor {cv::FaceDetectorYN::create(dnnWorkConf::faceDetOnnxPath, "", cv::Size(200, 300))};
            auto faceRec {cv::FaceRecognizerSF::create(dnnWorkConf::faceRecOnnxPath, "")};

            cv::Mat ftr(dnnWorkConf::getFeature(faceDtor, faceRec, img));

            for (auto const & e : features) {
                if (dnnWorkConf::isSameEntity(faceRec, ftr, e.second)) {
                    {
                        co::MutexGuard guard(outputMut);

                        std::cout << "\033[0;32m YOU ARE " << e.first << '\n';
                    }

                    {
                        co::MutexGuard guard(mut);

                        if (lastLogInsertUser == e.first && epoch::ms() - lastLogInsertTime < 12000) {
                            LOG << "log insert failed for time delaying!\n";

                            break;
                        } else {
                            lastLogInsertUser = e.first;
                            lastLogInsertTime = epoch::ms();
                        }
                    }

                    http::Client cliForSelect(httpHeaderConf::gdUserSqlServerUrl);

                    cliForSelect.add_header(httpHeaderConf::kOperation, httpHeaderConf::kSelect);
                    cliForSelect.add_header(httpHeaderConf::kName, e.first.c_str());

                    cliForSelect.post("/", "");

                    if (cliForSelect.response_code() != 200) {
                        LOG << "select req returned not 200!";

                        break;
                    }

                    if (std::string_view(cliForSelect.header(httpHeaderConf::kResult)) == httpHeaderConf::kFailed) {
                        LOG << "cliForSelect post result is failed! : "
                            << "reason is " << cliForSelect.header(httpHeaderConf::kReason);

                        break;
                    }

                    json::Json userInfo(json::parse(cliForSelect.body()).get("userList").get(0));

                    if (userInfo.is_null()) {
                        LOG << "userinfo json is null!";

                        break;
                    }

                    json::Json logJson(json::object());

                    logJson.add_member(httpHeaderConf::kName, e.first);
                    logJson.add_member(httpHeaderConf::kPhone, userInfo.get(httpHeaderConf::kPhone));
                    logJson.add_member(httpHeaderConf::kImageSrc, userInfo.get(httpHeaderConf::kImageSrc));
                    logJson.add_member(httpHeaderConf::kSite, pref::siteName);
                    logJson.add_member(httpHeaderConf::kTime, now::str());

                    http::Client cli(httpHeaderConf::gdLogSqlServerUrl);

                    cli.add_header(httpHeaderConf::kOperation, httpHeaderConf::kInsertLog);

                    cli.post("/", logJson.pretty().c_str());

                    if (cli.response_code() != 200) {
                        LOG << "response_code is not 200!";

                        break;
                    }

                    if (std::string_view(cli.header(httpHeaderConf::kResult)) == httpHeaderConf::kFailed) {
                        LOG << "cli result is failed! : "
                        << "reason is " << cli.header(httpHeaderConf::kReason);

                        break;
                    }

                    {
                        co::MutexGuard guard(outputMut);

                        LOG << "log insert  successful!\n";
                    }

                    break;
                }
            }
        }
    };
}