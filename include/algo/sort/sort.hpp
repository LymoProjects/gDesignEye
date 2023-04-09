#pragma once

#include <algorithm>
#include <functional>
#include <iterator>
#include <vector>

namespace lymoAlgo__ {
template <typename itType,
          typename cmpType =
              std::less<typename std::iterator_traits<itType>::value_type>>
auto insertionSort(itType begin, itType end, cmpType && cmp = cmpType{}) -> void {
    if (end - begin < 2) {
        return;
    }

    for (itType i{begin + 1}; i != end; ++i) {
        auto tmp{std::move(*i)};
        itType tmpPos{i};

        for (; tmpPos != begin; --tmpPos) {
            if (cmp(tmp, *(tmpPos - 1))) {
                *tmpPos = std::move(*(tmpPos - 1));
            } else {
                break;
            }
        }

        *tmpPos = std::move(tmp);
    }
}

template <typename itType,
          typename cmpType =
              std::less<typename std::iterator_traits<itType>::value_type>>
auto mergeSort(itType begin, itType end, cmpType && cmp = cmpType {}) -> void {
    if (end - begin < 2) {
        return;
    }

    itType mid {begin + ((end - begin) >> 1)};

    mergeSort(begin, mid, std::forward<cmpType>(cmp));
    mergeSort(mid, end, std::forward<cmpType>(cmp));

    if (cmp(*mid, *(mid - 1))) {
        std::vector<typename std::iterator_traits<itType>::value_type> tmp;

        for (itType i {begin}; i != end; ++i) {
            tmp.emplace_back(std::move(*i));
        }

        auto tmpMid {tmp.begin() + ((tmp.end() - tmp.begin()) >> 1)};
        auto leftBegin {tmp.begin()};
        auto rightBegin {tmpMid};

        for (itType i {begin}; i != end; ++i) {
            if (leftBegin == tmpMid) {
                *i = std::move(*rightBegin);

                ++rightBegin;
            } else if (rightBegin == tmp.end()) {
                *i = std::move(*leftBegin);

                ++leftBegin;
            } else {
                if (cmp(*leftBegin, *rightBegin)) {
                    *i = std::move(*leftBegin);

                    ++leftBegin;
                } else {
                    *i = std::move(*rightBegin);

                    ++rightBegin;
                }
            }
        }
    }
}
} // namespace lymoAlgo__