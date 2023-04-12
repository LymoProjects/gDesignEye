#pragma once

#include <string>

#include <co/fs.h>
#include <co/json.h>
#include <co/fastring.h>

namespace gd__ {
    struct pref {
        static constexpr char const * prefPath {"./pref.json"};

        static inline fastring siteName {"home"};

        static inline int captureIndex {};

        static
        auto init() -> bool {
            fs::file prefJsonFile(prefPath, 'r');

            if (!prefJsonFile) {
                return false;
            }

            json::Json prefJson(json::parse(prefJsonFile.read(prefJsonFile.size())));

            if (prefJson.is_null()) {
                return false;
            }

            siteName = prefJson.get("site").as_string();
            captureIndex = prefJson.get("capture").as_int();

            return true;
        }
    };
}