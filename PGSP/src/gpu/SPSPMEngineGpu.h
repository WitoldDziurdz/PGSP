#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <iostream>
#include <algorithm>
#include <array>
#include <sycl/sycl.hpp>

#include "../core/utils.h"
#include "../cpu/SimpleNode.h"
#include "../IEngine.h"
#include "../core/sycl_utiils.h"
#include "../core/profile.h"
#include "../core/helper_types.h"
#include "../core/data_parser.h"

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


            auto candidates = generate_k1_candidates(queue, data_base_);
            auto frequent_items = getFrequentItems(queue, data_buffer, ids_buffer, candidates);
            update(frequent_items);

            candidates = generate_size_2_candidates(frequent_items);
            frequent_items = getFrequentItems(queue, data_buffer, ids_buffer, candidates);
            update(frequent_items);

            size_t k = 3;
            while (!frequent_items.empty()) {
                candidates = generate_size_k_candidates(frequent_items, k);
                std::cout << k << " canditates: " << candidates.size() << std::endl;
                frequent_items = getFrequentItems(queue, data_buffer, ids_buffer, candidates);
                std::cout << k << " frequent_items: " << frequent_items.size() << std::endl;
                update(frequent_items);
                k++;
            }
        }
    private:

        inline std::vector<gsp::item> generate_k1_candidates(sycl::queue& queue, const std::vector<gsp::item>& data_base) {
            auto flat_data_base = convert(data_base);
            size_t nums_in_line = getMaxSizeOfLine(data_base);
            std::string_view data = flat_data_base.first;
            std::span<size_t> ids(flat_data_base.second.data(), flat_data_base.second.size());
            sycl::buffer<char, 1> data_buffer(data.data(), data.size());
            sycl::buffer<size_t, 1> ids_buffer(ids.data(), ids.size());
            const size_t rows = ids_buffer.size();
            const size_t cols = nums_in_line;
            constexpr size_t string_size = 1;
            FlatArray flat_candidates(rows, cols, string_size);

            sycl::buffer<char, 1> data_candidates_buffer(flat_candidates.data(), flat_candidates.size());
            sycl::buffer<size_t , 1> ids_candidates_buffer(flat_candidates.rowSizes(), flat_candidates.rows());

            queue.submit([&](sycl::handler& cgh) {
                sycl::accessor data_access{data_buffer, cgh, sycl::read_only};
                sycl::accessor ids_access{ids_buffer, cgh, sycl::read_only};
                sycl::accessor data_candidates_accessor{data_candidates_buffer, cgh, sycl::write_only};
                sycl::accessor ids_candidates_accessor{ids_candidates_buffer, cgh, sycl::write_only};
                cgh.parallel_for<class gen_can_k1>(sycl::range<1>(rows), [=](sycl::id<1> index) {
                    std::string_view gpu_data = std::string_view(data_access.get_pointer(), data_access.size());
                    sycl::span<size_t> gpu_ids(static_cast<size_t*>(ids_access.get_pointer()), ids_access.size());
                    DataBase gpu_dataBase(gpu_data, gpu_ids);

                    FlatArrayWrapper flat_candidates = FlatArrayWrapper(data_candidates_accessor.get_pointer(),
                                                                        ids_candidates_accessor.get_pointer(), rows, cols, string_size);
                    auto transaction = gpu_dataBase[index];
                    for (const auto& element : gsp::FlatElement(transaction)) {
                        for (char ch : element) {
                            std::string_view str(&ch, 1);
                            flat_candidates.insert(index, str);
                        }
                    }
                });
            });
            sycl::host_accessor data_candidates_accessor{data_candidates_buffer, sycl::read_write};
            sycl::host_accessor ids_candidates_accessor{ids_candidates_buffer, sycl::read_write};
            FlatArrayWrapper results_candidates = FlatArrayWrapper(data_candidates_accessor.get_pointer(),
                                                                ids_candidates_accessor.get_pointer(), rows, cols, string_size);
            std::set<gsp::item> candidates;
            for(size_t row = 0; row < results_candidates.rows(); row++){
                size_t max_col =  ids_candidates_accessor[row];
                for(size_t col = 0; col < max_col; col++){
                    std::string_view view = results_candidates.get(row, col);
                    gsp::item item;
                    item.reserve(view.size());
                    for(std::string_view el : gsp::FlatElement(view)){
                        item.emplace_back(el);
                    }
                    candidates.insert(item);
                }
            }
            return { candidates.begin(), candidates.end() };
        }

        inline gsp::map_items getFrequentItems(sycl::queue& queue, sycl::buffer<char, 1>& data_buffer, sycl::buffer<size_t, 1>& ids_buffer,
                                        std::vector<gsp::item>& candidates){
            if(candidates.empty()){
                std::cout << "List of candidates is empty!" << std::endl;
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
                    Accessor[index] = gpu::getCount(gpu_dataBase, candidate);
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