#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <iostream>
#include <algorithm>

#include <sycl/sycl.hpp>

#include "utils.h"
#include "SimpleNode.h"
#include "IEngine.h"
#include "sycl_utiils.h"
#include "profile.h"
#include "HelperTypes.h"

namespace gsp {
    class SPSPMEngineGpu : public IEngine {

    public:
        SPSPMEngineGpu(const std::vector<gsp::item>& data_base, size_t min_support, size_t num_of_work_group): 
            IEngine("Simply Partitioned Sequential Pattern Mining GPU", data_base, min_support),
            num_of_work_group_{ num_of_work_group } { 

        }

        void calculate() override {
            TotalDuration timer(name_ + " - total time speneded on calculating:");
            sycl::queue queue(sycl::default_selector_v);
            std::cout << "Running on " << queue.get_device().get_info<sycl::info::device::name>() << "\n";;

            //TO DO remove
            auto flat_data_base = convert(data_base_);
            std::string_view data = flat_data_base.first;
            std::span<size_t> ids(flat_data_base.second.data(), flat_data_base.second.size());
            sycl::buffer<char, 1> data_buffer(data.data(), data.size());
            sycl::buffer<size_t, 1> ids_buffer(ids.data(), ids.size());

            auto candidates = generate_size_1_candidates(data_base_);
            auto frequent_items = getFrequentItems(queue, data_buffer, ids_buffer, candidates);
            update(frequent_items);

            candidates = generate_size_2_candidates(frequent_items);
            frequent_items = getFrequentItems(queue, data_buffer, ids_buffer, candidates);
            update(frequent_items);

            size_t k = 3;
            while (!frequent_items.empty()) {
                candidates = generate_size_k_candidates(frequent_items, k);
                std::cout << k << " canditates: " << candidates.size() << std::endl;
                if(candidates.empty()){
                    break;
                }
                frequent_items = getFrequentItems(queue, data_buffer, ids_buffer, candidates);
                update(frequent_items);
                k++;
            }
        }
    private:

        inline gsp::map_items getFrequentItems(sycl::queue& queue, sycl::buffer<char, 1>& data_buffer, sycl::buffer<size_t, 1>& ids_buffer,
                                        std::vector<gsp::item>& candidates){
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
            queue.wait();

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
            return convertToFrequentItems(candidates, nums);
        }

        gsp::map_items convertToFrequentItems(std::vector<gsp::item>& candidates, std::vector<size_t>& nums){
            gsp::map_items frequent_item;
            for(size_t i = 0; i < nums.size(); ++i){
                if(nums[i] >= min_support_) {
                    frequent_item.insert({candidates[i], nums[i]});
                }
            }
            return frequent_item;
        }

        const size_t num_of_work_group_;
    };
}