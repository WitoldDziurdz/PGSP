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

namespace gsp {

    using item = std::vector<std::string>;
    bool isContain(const std::string& item, const std::string& sub_item);

    bool isSubSequence(const std::vector<std::string>& seq, const std::vector<std::string>& sub_seq);

    std::set<char> getUniqItems(const std::vector<std::string>& sequence);

    std::vector <std::vector<item>> split(const std::vector<item>& vec, size_t base);

    std::set<char> generateUniqItems(const std::vector<gsp::item>& data_base);

    template<typename T>
    std::vector<std::vector<T>> split(const std::vector<T>& input, size_t n) {
        std::vector<std::vector<T>> result;

        for (size_t i = 0; i < input.size(); i += n) {
            std::vector<T> chunk(input.begin() + i, input.begin() + std::min(i + n, input.size()));
            result.push_back(chunk);
        }

        return result;
    }

    std::vector<gsp::item> generate_size_1_candidates(const std::vector<gsp::item>& database);

    std::vector<gsp::item> generate_size_2_candidates(const std::map<gsp::item, size_t>& frequent_items);

    std::string flatItem(const gsp::item& element);

    std::string deleteFirstElement(const gsp::item& element);

    std::string deleteLastElement(const gsp::item& element);

    std::string getLastElement(const gsp::item& element);

    bool needMerge(const gsp::item& element1, const gsp::item& element2);

    size_t getSize(const gsp::item& item);

    std::vector<gsp::item> generate_size_k_candidates(const std::map<gsp::item, size_t>& frequent_items, size_t k);

    void filter(std::map<gsp::item, size_t>& frequency, size_t min_support);

    std::map<gsp::item, size_t> getFrequentItems(const std::vector<gsp::item>& data_base, std::vector<item>& candidates);

    bool isCanBeFrequent(const std::set<std::string>& frequent_items, const gsp::item& candidate);

    std::vector<gsp::item> prune(const std::map<gsp::item, size_t>& frequent_items, const std::vector <gsp::item>& candidates);

    std::ostream& operator<<(std::ostream& os, const item& item);

    std::ostream& operator<<(std::ostream& os, const std::pair<gsp::item, size_t>& item);

    std::ostream& operator<<(std::ostream& os, const std::vector<std::pair<gsp::item, size_t>>& items);

    void print(const std::vector<std::pair<gsp::item, size_t>>& items);

    void writeToFile(const std::vector<std::pair<gsp::item, size_t>>& items, std::string fileName);
}