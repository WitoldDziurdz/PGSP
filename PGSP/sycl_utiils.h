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

#include "HelperTypes.h"
#include "utils.h"

namespace gsp {

    std::vector<gsp::item> generate_size_1_candidates(const std::string& database) {
        std::set<std::string> candidates;
        for (const auto& transaction : database) {
            for (const auto& element : transaction) {
                for (const auto& ev : element) {
                    std::string str;
                    str += ev;
                    gsp::item seq = { str };
                    candidates.insert(seq);
                }
            }
        }
        return { candidates.begin(), candidates.end() };
    }

}