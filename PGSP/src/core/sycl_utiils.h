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

    inline void sort(char* begin, char* end) {
        for (char* i = begin; i != end; ++i) {
            for (char* j = begin; j != (end - 1); ++j) {
                if (*j > *(j + 1)) {
                    std::swap(*j, *(j + 1));
                }
            }
        }
    }

    inline size_t find(std::string_view str_view, char ch) {
        for (size_t i = 0; i < str_view.size(); ++i) {
            if (str_view[i] == ch) {
                return i;
            }
        }
        return std::string_view::npos;
    }

    inline size_t rfind(std::string_view str_view, char ch) {
        for (size_t i = str_view.size(); i > 0; --i) {
            if (str_view[i - 1] == ch) {
                return i - 1;
            }
        }
        return std::string_view::npos;
    }


    inline bool isContain(const std::string_view item, const std::string_view sub_item) {
        for (char c : sub_item) {
            if (std::count(item.begin(), item.end(), c) < std::count(sub_item.begin(), sub_item.end(), c)) {
                return false;
            }
        }
        return true;
    }

    inline std::vector<std::string> generate_size_1_candidates(DataBase database) {
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

    inline bool isSubSequence(std::string_view line, std::string_view sub_line) {
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

    void merge(char* data, string_view str, char ch, size_t string_size){
        size_t i = 0;
        for(i = 0; i < str.size(); ++i){
            data[i] = str[i];
        }
        size_t begin = rfind(str, ',');
        if(begin == std::string_view::npos){
            begin = 0;
        }
        data[i++] = ch;
        sort(data + begin, data + i);
        for(;i < string_size; ++i){
            data[i] = ' ';
        }
    }

    void insert(char* data, string_view str, char ch, size_t string_size){
        size_t i = 0;
        for(i = 0; i < str.size(); ++i){
            data[i] = str[i];
        }
        data[i++] = ',';
        data[i++] = ch;
        for(;i < string_size; ++i){
            data[i] = ' ';
        }
    }

    bool isCanBeCandidate(std::string_view first, std::string_view second) {
        return true;
    }

    size_t getRealSize(std::string_view str){
        size_t i = 0;
        for(i = 0; i < str.size(); ++i){
            if(str[i] == ' '){
                return i+1;
            }
        }
        return i+1;
    }

    inline size_t getHash(ItemIteartor it, size_t k) {
        size_t sum = 0;
        constexpr size_t offset = 8;
        for (size_t i = 0; i < k; ++i) {
            sum += (static_cast<size_t>(*it) << offset);
            ++it;
        }
        return sum;
    }

    inline size_t getHash(std::string_view str, size_t k) {
        size_t sum = 0;
        constexpr size_t offset = 8;
        for (size_t i = 0; i < k; ++i) {
            sum += (static_cast<size_t>(str[i]) << offset);
        }
        return sum;
    }

    bool isMine(size_t h, size_t index, size_t num_of_work_group){
        return ((h % num_of_work_group) == index);
    }

    inline size_t getId(char ch, size_t num_of_work_group) {
        return static_cast<size_t>(ch) % num_of_work_group;
    }

    inline size_t getCount(DataBase& gpu_dataBase, std::string_view candidate){
        size_t support = 0;
        for(size_t j = 0; j < gpu_dataBase.size(); ++j) {
            auto line = gpu_dataBase[j];
            if (gsp::gpu::isSubSequence(line, candidate)) {
                support++;
            }
        }
        return support;
    }

}
}