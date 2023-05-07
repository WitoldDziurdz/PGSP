#pragma once
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <fstream>
#include <unordered_map>

#include "helper_types.h"
#include "utils.h"

namespace gsp {
namespace gpu {

    template<typename Iterator>
    void sort(Iterator begin, Iterator end) {
        if (begin == end || std::next(begin) == end) {
            return;
        }

        bool swapped;
        Iterator last_unsorted = end;

        do {
            swapped = false;
            Iterator current = begin;

            for (Iterator next = std::next(current); next != last_unsorted; current = next, ++next) {
                if (*next < *current) {
                    std::iter_swap(current, next);
                    swapped = true;
                }
            }

            last_unsorted = current;
        } while (swapped);
    }


    bool isContain(const std::string_view item, const std::string_view sub_item) {
        for (char c : sub_item) {
            if (std::count(item.begin(), item.end(), c) < std::count(sub_item.begin(), sub_item.end(), c)) {
                return false;
            }
        }
        return true;
    }

    std::vector<std::string> generate_size_1_candidates(DataBase database) {
        std::set<std::string> candidates;
        for (const auto &transaction: database) {
            for (const auto &element: gsp::FlatElement(transaction)) {
                for (const auto &ev: element) {
                    std::string str;
                    str += ev;
                    candidates.insert(str);
                }
            }
        }
        std::vector<std::string> result(candidates.begin(), candidates.end());
        result.shrink_to_fit();
        return result;
    }

    bool isSubSequence(std::string_view line, std::string_view sub_line) {
        gsp::FlatElement seq(line);
        gsp::FlatElement sub_seq(sub_line);
        if (seq.size() < sub_seq.size()) {
            return false;
        }
        for (auto it = seq.begin(), it_sub = sub_seq.begin(); it != seq.end(); ++it) {
            if (isContain(*it, *it_sub)) {
                ++it_sub;
            }
            if (it_sub == sub_seq.end()) {
                return true;
            }
        }
        return false;
    }

}
}