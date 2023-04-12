#pragma once

namespace gd__ {
    struct httpHeaderConf {
        static constexpr char const * gdUserSqlServerUrl {"http://localhost:9190"};
        static constexpr char const * gdLogSqlServerUrl {"http://localhost:9192"};
        static constexpr char const * featureBinServerUrl {"http://localhost:9194"};

        static constexpr char const * kOperation {"operation"};
        static constexpr char const * kResult {"result"};
        static constexpr char const * kSuccess {"success"};
        static constexpr char const * kFailed {"failed"};
        static constexpr char const * kReason {"reason"};

        static constexpr char const * kName {"name"};
        static constexpr char const * kPhone {"phone"};
        static constexpr char const * kImageSrc {"imagesrc"};
        static constexpr char const * kSite {"site"};
        static constexpr char const * kTime {"time"};

        static constexpr char const * kSelect {"select"};
        static constexpr char const * kInsertLog {"insertlog"};
    };
}