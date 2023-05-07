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

namespace gsp {

    bool isContain(const std::string_view item, const std::string_view sub_item);

    bool isSubSequence(const std::vector<std::string>& seq, const std::vector<std::string>& sub_seq);

    std::vector<std::vector<item>> split(std::vector<gsp::item> items, size_t num_of_work_group);

    std::vector<gsp::item> generate_size_1_candidates(const std::vector<gsp::item>& database);

    std::vector<gsp::item> generate_size_2_candidates(const map_items& frequent_items);

    std::string flatItem(const gsp::item& element);

    bool isCanBeCandidate(const gsp::item& first, const gsp::item& second);

    bool needMerge(const gsp::item& element1, const gsp::item& element2);

    size_t getSize(const gsp::item& item);

    std::vector<gsp::item> generate_size_k_candidates(const map_items& frequent_items, size_t k);

    void filter(map_items& frequency, size_t min_support);

    map_items getFrequentItems(const std::vector<gsp::item>& data_base, std::vector<item>& candidates);

    bool isCanBeFrequent(const std::set<std::string>& frequent_items, const gsp::item& candidate);

    std::vector<gsp::item> prune(const map_items& frequent_items, const std::vector <gsp::item>& candidates);

    std::ostream& operator<<(std::ostream& os, const item& item);

    std::ostream& operator<<(std::ostream& os, const std::pair<gsp::item, size_t>& item);

    std::ostream& operator<<(std::ostream& os, const std::vector<std::pair<gsp::item, size_t>>& items);

    void print(const std::vector<std::pair<gsp::item, size_t>>& items);

    void writeToFile(const std::vector<std::pair<gsp::item, size_t>>& items, std::string fileName);

}