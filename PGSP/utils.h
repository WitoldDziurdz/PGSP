#pragma once
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <sstream>
#include <numeric>
#include <iostream>
#include <fstream>
#include <unordered_map>

namespace gsp {

    struct VectorOfStringHash {
        std::size_t operator()(const std::vector<std::string>& vec) const {
            std::size_t seed = 0;
            std::hash<std::string> hasher;
            for (const auto& str : vec) {
                seed ^= hasher(str) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };

    using item = std::vector<std::string>;
    using map_items = std::unordered_map<std::vector<std::string>, int, VectorOfStringHash>;

    bool isContain(const std::string_view item, const std::string_view sub_item);

    bool isSubSequence(const std::vector<std::string>& seq, const std::vector<std::string>& sub_seq);

    std::set<char> getUniqItems(const std::vector<std::string>& sequence);

    std::vector<std::vector<item>> split(std::vector<gsp::item> items, size_t num_of_work_group);

    std::set<char> generateUniqItems(const std::vector<gsp::item>& data_base);

    std::vector<gsp::item> generate_size_1_candidates(const std::vector<gsp::item>& database);

    std::vector<gsp::item> generate_size_2_candidates(const map_items& frequent_items);

    std::string flatItem(const gsp::item& element);

    std::string deleteFirstElement(const gsp::item& element);

    std::string deleteLastElement(const gsp::item& element);
    bool isCanBecandidate(const gsp::item& first, const gsp::item& second);
    std::string getLastElement(const gsp::item& element);

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