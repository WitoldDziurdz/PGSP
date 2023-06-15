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

    inline bool isContain(const std::string_view item, const std::string_view sub_item) {
        for (char c : sub_item) {
            if (std::count(item.begin(), item.end(), c) < std::count(sub_item.begin(), sub_item.end(), c)) {
                return false;
            }
        }
        return true;
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

    inline size_t getHash(std::string_view str, size_t k) {
        size_t sum = 0;
        for (size_t i = 0; i < k; ++i) {
            if(str[i] == ',' || str[i] == ' ' || str[i] =='.'){
                continue;
            }
            sum  = sum << 1;
            sum += static_cast<size_t>(str[i]);
        }
        return sum;
    }

    inline gsp::map_items convertToFrequentItems(std::vector<gsp::item>& candidates, std::vector<size_t>& nums, size_t min_support){
        gsp::map_items frequent_item;
        for(size_t i = 0; i < nums.size(); ++i){
            if(nums[i] >= min_support) {
                frequent_item.insert({candidates[i], nums[i]});
            }
        }
        return frequent_item;
    }

    inline gsp::map_items getFrequentItems(sycl::queue& queue, sycl::buffer<char, 1>& data_buffer, sycl::buffer<size_t, 1>& ids_buffer,
                                           std::vector<gsp::item>& candidates, size_t min_support){
        if(candidates.empty()){
            //std::cout << "List of candidates is empty!" << std::endl;
            return {};
        }
        auto flat_candidates = convert(candidates);
        std::string_view data_candidates = flat_candidates.first;
        std::span<size_t> ids_candidates(flat_candidates.second.data(), flat_candidates.second.size());
        sycl::buffer<size_t, 1> Buffer(ids_candidates.size());


        sycl::buffer<char, 1> data_candidates_buffer(data_candidates.data(), data_candidates.size());
        sycl::buffer<size_t, 1> ids_candidates_buffer(ids_candidates.data(), ids_candidates.size());

        queue.submit([&](sycl::handler& cgh) {
            sycl::accessor Accessor{Buffer, cgh, sycl::write_only};
            cgh.parallel_for<class SPSPM_k_0>(sycl::range<1>(Accessor.size()), [=](sycl::id<1> index) {
                Accessor[index] = 0;
            });
        });

        queue.submit([&](sycl::handler& cgh) {
            sycl::accessor data_access{data_buffer, cgh, sycl::read_only};
            sycl::accessor ids_access{ids_buffer, cgh, sycl::read_only};
            sycl::accessor data_candidates_access{data_candidates_buffer, cgh, sycl::read_only};
            sycl::accessor ids_candidates_access{ids_candidates_buffer, cgh, sycl::read_only};
            sycl::accessor Accessor{Buffer, cgh, sycl::write_only};

            cgh.parallel_for<class SPSPM_k_1>(sycl::range<1>(Accessor.size()), [=](sycl::id<1> index) {
                std::string_view gpu_data = std::string_view(data_access.get_pointer(), data_access.size());
                sycl::span<size_t> gpu_ids(static_cast<size_t*>(ids_access.get_pointer()), ids_access.size());
                DataBase gpu_dataBase(gpu_data, gpu_ids);

                std::string_view gpu_data_candidates = std::string_view(data_candidates_access.get_pointer(), data_candidates_access.size());
                sycl::span<size_t> gpu_ids_candidates(static_cast<size_t*>(ids_candidates_access.get_pointer()), ids_candidates_access.size());
                DataBase gpu_candidates(gpu_data_candidates, gpu_ids_candidates);

                auto candidate = gpu_candidates[index];
                for(auto line : gpu_dataBase){
                    if(gsp::gpu::isSubSequence(line, candidate)){
                        Accessor[index]++;
                    }
                }
            });
        });
        sycl::host_accessor HostAccessor{Buffer, sycl::read_only};
        std::vector<size_t> nums;
        nums.reserve(HostAccessor.size());
        for(size_t i = 0; i < HostAccessor.size(); ++i){
            nums.push_back(HostAccessor[i]);
        }
        return convertToFrequentItems(candidates, nums, min_support);
    }
}
}