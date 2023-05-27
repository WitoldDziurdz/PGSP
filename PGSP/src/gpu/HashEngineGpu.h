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
    class HashEngineGpu : public IEngine {

    public:
        HashEngineGpu(const std::vector<gsp::item>& data_base, size_t min_support, size_t num_of_work_group):
                IEngine("Hash Partitioned Sequential Pattern Mining GPU", data_base, min_support),
                num_of_work_group_{ num_of_work_group } {

        }

        void calculate() override {
            TotalDuration timer(name_ + " - total time speneded on calculating:");
            sycl::queue queue(sycl::default_selector_v);
            std::cout << "Running on " << queue.get_device().get_info<sycl::info::device::name>() << "\n";;

            auto flat_data_base = convert(data_base_);
            size_t nums_in_line = getMaxSizeOfLine(data_base_);
            std::string_view data = flat_data_base.first;
            std::span<size_t> ids(flat_data_base.second.data(), flat_data_base.second.size());
            sycl::buffer<char, 1> data_buffer(data.data(), data.size());
            sycl::buffer<size_t, 1> ids_buffer(ids.data(), ids.size());
            const size_t inner_rows = ids.size();
            const size_t inner_cols = nums_in_line;

            size_t k = 1;
            auto frequent_items = iter_k1(queue, data_buffer, ids_buffer, inner_rows,  inner_cols, k);
            update(frequent_items);
            std::cout << k << " frequent_items: " << frequent_items.size() << std::endl;
            k = 2;
            frequent_items = iter_k2(queue, data_buffer, ids_buffer, inner_rows,  inner_cols, convert(frequent_items), k);
            update(frequent_items);
            std::cout << k << " frequent_items: " << frequent_items.size() << std::endl;
            k = 3;
            while (!frequent_items.empty()) {
                frequent_items = iter_k(queue, data_buffer, ids_buffer, inner_rows,  inner_cols, convert(frequent_items), k);
                std::cout << k << " frequent_items: " << frequent_items.size() << std::endl;
                update(frequent_items);
                k++;
            }
        }
    private:

        inline map_items iter_k1(sycl::queue& queue, sycl::buffer<char, 1>& data_buffer, sycl::buffer<size_t, 1>& ids_buffer, size_t inner_rows, size_t inner_cols, size_t k) {
            const size_t rows = num_of_work_group_;
            const size_t cols = inner_rows * inner_cols;
            size_t string_size = 2*k - 1;
            FlatArray flat_candidates(rows, cols, string_size);

            sycl::buffer<char, 1> data_candidates_buffer(flat_candidates.data(), flat_candidates.size());
            sycl::buffer<size_t, 1> ids_candidates_buffer(flat_candidates.rowSizes(), flat_candidates.rows());
            sycl::range<2> range_value(rows, cols);
            sycl::buffer<size_t, 2> nums_buffer(range_value);

            queue.submit([&](sycl::handler& cgh) {
                sycl::accessor data_access{data_buffer, cgh, sycl::read_only};
                sycl::accessor ids_access{ids_buffer, cgh, sycl::read_only};

                sycl::accessor data_candidates_accessor{data_candidates_buffer, cgh, sycl::read_write};
                sycl::accessor ids_candidates_accessor{ids_candidates_buffer, cgh, sycl::read_write};
                sycl::accessor nums{nums_buffer, cgh, sycl::read_write};

                cgh.parallel_for<class Hash_k1>(sycl::range<1>(rows), [=](sycl::id<1> index) {
                    std::string_view gpu_data = std::string_view(data_access.get_pointer(), data_access.size());
                    sycl::span<size_t> gpu_ids(static_cast<size_t*>(ids_access.get_pointer()), ids_access.size());
                    DataBase gpu_dataBase(gpu_data, gpu_ids);
                    FlatArrayWrapper flat_candidates = FlatArrayWrapper(data_candidates_accessor.get_pointer(),
                                                                        ids_candidates_accessor.get_pointer(), rows, cols, string_size);

                    for(size_t i = 0; i < gpu_dataBase.size(); ++i) {
                        auto transaction = gpu_dataBase[i];
                        for (const auto &element: gsp::FlatElement(transaction)) {
                            for (char candidate: element) {
                                if(!gpu::isMine(static_cast<size_t>(candidate), index, rows)){
                                    continue;
                                }
                                std::string_view str(&candidate, 1);
                                flat_candidates.insert(index, str);
                            }
                        }
                    }
                    for(size_t i = 0; i < flat_candidates.size(index); ++i) {
                        std::string_view candidate = flat_candidates.get(index, i);
                        nums[index][i] = gpu::getCount(gpu_dataBase, candidate);
                    }
                });
            });

            sycl::host_accessor data_candidates_accessor{data_candidates_buffer, sycl::read_write};
            sycl::host_accessor ids_candidates_accessor{ids_candidates_buffer, sycl::read_write};
            sycl::host_accessor nums_accessor{nums_buffer, sycl::read_write};
            FlatArrayWrapper results_candidates = FlatArrayWrapper(data_candidates_accessor.get_pointer(),
                                                                   ids_candidates_accessor.get_pointer(), rows, cols, string_size);
            return convertToMapItems(results_candidates, nums_accessor);
        }

        inline map_items iter_k2(sycl::queue& queue, sycl::buffer<char, 1>& data_buffer, sycl::buffer<size_t, 1>& ids_buffer, size_t inner_rows, size_t inner_cols, const std::vector<gsp::item>& frequent_items, size_t k) {
            auto flat_frequent_items = convert(frequent_items);
            std::string_view data_frequent_items = flat_frequent_items.first;
            std::span<size_t> ids_frequent_items(flat_frequent_items.second.data(), flat_frequent_items.second.size());
            sycl::buffer<char, 1> data_frequent_items_buffer(data_frequent_items.data(), data_frequent_items.size());
            sycl::buffer<size_t, 1> ids_frequent_items_buffer(ids_frequent_items.data(), ids_frequent_items.size());
            const size_t rows = num_of_work_group_;
            const size_t cols = inner_rows * inner_cols;
            size_t string_size = 2*k - 1;
            FlatArray flat_candidates(rows, cols, string_size);

            sycl::buffer<char, 1> data_candidates_buffer(flat_candidates.data(), flat_candidates.size());
            sycl::buffer<size_t, 1> ids_candidates_buffer(flat_candidates.rowSizes(), flat_candidates.rows());
            sycl::range<2> range_value(rows, cols);
            sycl::buffer<size_t, 2> nums_buffer(range_value);

            sycl::range<2> range_temp(rows, string_size);
            sycl::buffer<char, 2> temp_candidate_buffer(range_temp);

            queue.submit([&](sycl::handler& cgh) {
                sycl::accessor data_access{data_buffer, cgh, sycl::read_only};
                sycl::accessor ids_access{ids_buffer, cgh, sycl::read_only};

                sycl::accessor data_frequent_items_access{data_frequent_items_buffer, cgh, sycl::read_only};
                sycl::accessor ids_frequent_items_access{ids_frequent_items_buffer, cgh, sycl::read_only};

                sycl::accessor data_candidates_accessor{data_candidates_buffer, cgh, sycl::read_write};
                sycl::accessor ids_candidates_accessor{ids_candidates_buffer, cgh, sycl::read_write};
                sycl::accessor nums{nums_buffer, cgh, sycl::read_write};

                sycl::accessor temp_candidate_accessor{temp_candidate_buffer, cgh, sycl::read_write};

                cgh.parallel_for<class Hash_k2>(sycl::range<1>(rows), [=](sycl::id<1> index) {
                    std::string_view gpu_data = std::string_view(data_access.get_pointer(), data_access.size());
                    sycl::span<size_t> gpu_ids(static_cast<size_t*>(ids_access.get_pointer()), ids_access.size());
                    DataBase gpu_dataBase(gpu_data, gpu_ids);

                    std::string_view frequent_items_gpu_data = std::string_view(data_frequent_items_access.get_pointer(), data_frequent_items_access.size());
                    sycl::span<size_t> frequent_items_gpu_ids(static_cast<size_t*>(ids_frequent_items_access.get_pointer()), ids_frequent_items_access.size());
                    DataBase gpu_frequent_items(frequent_items_gpu_data, frequent_items_gpu_ids);

                    FlatArrayWrapper flat_candidates = FlatArrayWrapper(data_candidates_accessor.get_pointer(),
                                                                        ids_candidates_accessor.get_pointer(), rows, cols, string_size);

                    for(size_t i = 0; i < gpu_frequent_items.size(); ++i) {
                        auto transaction1 = gpu_frequent_items[i];
                        char ch1 = transaction1[0];
                        if(!gpu::isMine(static_cast<size_t>(ch1), index, rows)){
                            continue;
                        }
                        for(size_t j = 0; j < gpu_frequent_items.size(); ++j) {
                            auto transaction2 = gpu_frequent_items[j];
                            char ch2 = transaction2[0];
                            std::array<char, 3> tmp;

                            if(ch1 != ch2){
                                gpu::merge(tmp.data(), transaction1, ch2, flat_candidates.string_size());
                                std::string_view temp_str1 = std::string_view(temp_candidate_accessor.get_pointer(), flat_candidates.string_size());

                                flat_candidates.insert(index, temp_str1);
                            }
                            gpu::insert(tmp.data(), transaction1, ch2, flat_candidates.string_size());
                            std::string_view temp_str2 = std::string_view(temp_candidate_accessor.get_pointer(), flat_candidates.string_size());
                            flat_candidates.insert(index, temp_str2);
                        }
                    }

                    for(size_t i = 0; i < flat_candidates.size(index); ++i) {
                        std::string_view candidate = flat_candidates.get(index, i);
                        nums[index][i] = gpu::getCount(gpu_dataBase, candidate);
                    }
                });
            });

            sycl::host_accessor data_candidates_accessor{data_candidates_buffer, sycl::read_write};
            sycl::host_accessor ids_candidates_accessor{ids_candidates_buffer, sycl::read_write};
            sycl::host_accessor nums_accessor{nums_buffer, sycl::read_write};
            FlatArrayWrapper results_candidates = FlatArrayWrapper(data_candidates_accessor.get_pointer(),
                                                                   ids_candidates_accessor.get_pointer(), rows, cols, string_size);
            return convertToMapItems(results_candidates, nums_accessor);
        }

        inline map_items iter_k(sycl::queue& queue, sycl::buffer<char, 1>& data_buffer, sycl::buffer<size_t, 1>& ids_buffer, size_t inner_rows, size_t inner_cols, const std::vector<gsp::item>& frequent_items, size_t k) {

            auto flat_frequent_items = convert(frequent_items);
            std::string_view data_frequent_items = flat_frequent_items.first;
            std::span<size_t> ids_frequent_items(flat_frequent_items.second.data(), flat_frequent_items.second.size());
            sycl::buffer<char, 1> data_frequent_items_buffer(data_frequent_items.data(), data_frequent_items.size());
            sycl::buffer<size_t, 1> ids_frequent_items_buffer(ids_frequent_items.data(), ids_frequent_items.size());

            const size_t rows = num_of_work_group_;
            const size_t cols = 20*inner_rows * inner_cols ;
            size_t string_size = 2*k - 1;
            FlatArray flat_candidates(rows, cols, string_size);

            sycl::buffer<char, 1> data_candidates_buffer(flat_candidates.data(), flat_candidates.size());
            sycl::buffer<size_t, 1> ids_candidates_buffer(flat_candidates.rowSizes(), flat_candidates.rows());
            sycl::range<2> range_value(rows, cols);
            sycl::buffer<size_t, 2> nums_buffer(range_value);

            sycl::range<2> range_temp(rows, string_size);
            sycl::buffer<char, 2> temp_candidate_buffer(range_temp);

            queue.submit([&](sycl::handler& cgh) {
                sycl::accessor data_access{data_buffer, cgh, sycl::read_only};
                sycl::accessor ids_access{ids_buffer, cgh, sycl::read_only};

                sycl::accessor data_frequent_items_access{data_frequent_items_buffer, cgh, sycl::read_only};
                sycl::accessor ids_frequent_items_access{ids_frequent_items_buffer, cgh, sycl::read_only};

                sycl::accessor data_candidates_accessor{data_candidates_buffer, cgh, sycl::read_write};
                sycl::accessor ids_candidates_accessor{ids_candidates_buffer, cgh, sycl::read_write};
                sycl::accessor nums{nums_buffer, cgh, sycl::write_only};

                sycl::accessor temp_candidate_accessor{temp_candidate_buffer, cgh, sycl::read_write};

                cgh.parallel_for<class Hash_k>(sycl::range<1>(rows), [=](sycl::id<1> index) {
                    std::string_view gpu_data = std::string_view(data_access.get_pointer(), data_access.size());
                    sycl::span<size_t> gpu_ids(static_cast<size_t*>(ids_access.get_pointer()), ids_access.size());
                    DataBase gpu_dataBase(gpu_data, gpu_ids);

                    std::string_view frequent_items_gpu_data = std::string_view(data_frequent_items_access.get_pointer(), data_frequent_items_access.size());
                    sycl::span<size_t> frequent_items_gpu_ids(static_cast<size_t*>(ids_frequent_items_access.get_pointer()), ids_frequent_items_access.size());
                    DataBase gpu_frequent_items(frequent_items_gpu_data, frequent_items_gpu_ids);

                    FlatArrayWrapper flat_candidates = FlatArrayWrapper(data_candidates_accessor.get_pointer(),
                                                                        ids_candidates_accessor.get_pointer(), rows, cols, string_size);

                    for(size_t i = 0; i < gpu_frequent_items.size(); ++i) {
                        auto transaction1 = gpu_frequent_items[i];
                        std::string_view sub_transaction1 = std::string_view(transaction1.begin() + 1, transaction1.size() - 1);
                        auto h1 = gpu::getHash(sub_transaction1, sub_transaction1.size());
                        if(!gpu::isMine(h1, index, rows)){
                            continue;
                        }
                        char *tmp = &temp_candidate_accessor[index][0];
                        for(size_t j = 0; j < gpu_frequent_items.size(); ++j) {
                            auto transaction2 = gpu_frequent_items[j];
                            std::string_view sub_transaction2 = std::string_view(transaction2.begin(), transaction2.size() - 1);
                            auto h2 = gpu::getHash(sub_transaction2, sub_transaction2.size());
                            if((h1 == h2) && (gpu::isCanBeCandidate(sub_transaction1, sub_transaction2))) {
                                char ch = transaction2.back();

                                gpu::merge(tmp, transaction1, ch, flat_candidates.string_size());
                                std::string_view temp_str1 = std::string_view(temp_candidate_accessor.get_pointer(), temp_candidate_accessor.size());
                                flat_candidates.insert(index, temp_str1);

                                gpu::insert(tmp, transaction1, ch, flat_candidates.string_size());
                                std::string_view temp_str2 = std::string_view(temp_candidate_accessor.get_pointer(), temp_candidate_accessor.size());
                                flat_candidates.insert(index, temp_str2);
                            }
                        }
                    }

                    for(size_t i = 0; i < flat_candidates.size(index); ++i) {
                        std::string_view candidate = flat_candidates.get(index, i);
                        nums[index][i] = gpu::getCount(gpu_dataBase, candidate);
                    }
                });
            });

            sycl::host_accessor data_candidates_accessor{data_candidates_buffer, sycl::read_write};
            sycl::host_accessor ids_candidates_accessor{ids_candidates_buffer, sycl::read_write};
            sycl::host_accessor nums_accessor{nums_buffer, sycl::read_write};
            FlatArrayWrapper results_candidates = FlatArrayWrapper(data_candidates_accessor.get_pointer(),
                                                                   ids_candidates_accessor.get_pointer(), rows, cols, string_size);
            return convertToMapItems(results_candidates, nums_accessor);
        }

        map_items convertToMapItems(FlatArrayWrapper& results_candidates, sycl::host_accessor<size_t, 2>& nums_accessor){
            map_items candidates;
            for(size_t row = 0; row < results_candidates.rows(); row++){
                size_t max_col =  results_candidates.size(row);
                for(size_t col = 0; col < max_col; col++){
                    std::string_view view = results_candidates.get(row, col);
                    gsp::item item;
                    item.reserve(view.size());
                    for(std::string_view el : gsp::FlatElement(view)){
                        item.emplace_back(el);
                    }
                    size_t support = nums_accessor[row][col];
                    if(support >= min_support_) {
                        auto [it, status] = candidates.insert({item, support});
                        if(!status){
                            std::cout << "error: " << view << std::endl;
                        }
                    }
                }
            }
            return candidates;
        }

        const size_t num_of_work_group_;
    };
}