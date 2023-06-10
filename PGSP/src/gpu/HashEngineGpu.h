#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <iostream>
#include <algorithm>
#include <array>
#include <sstream>

#include <sycl/sycl.hpp>

#include "../core/utils.h"
#include "../gpu/HashNodeSycl.h"
#include "../IEngine.h"
#include "../core/sycl_utiils.h"
#include "../core/profile.h"
#include "../core/helper_types.h"
#include "../core/data_parser.h"

namespace gsp {
    class HashEngineGpu : public IEngine {

    public:
        HashEngineGpu(const std::vector<gsp::item>& data_base, size_t min_support, size_t num_of_work_group, bool info_logs = false, bool debug_logs = false):
                IEngine("Hash Partitioned Sequential Pattern Mining GPU", data_base, min_support, info_logs, debug_logs),
                num_of_work_group_{ num_of_work_group } {
            for (size_t i = 0; i < num_of_work_group_; ++i) {
                nodes_.emplace_back(data_base, i, min_support, num_of_work_group);
            }
        }

        void calculate() override {
            sycl::queue queue(sycl::default_selector_v);
            TotalDuration timer(name_  + " " + queue.get_device().get_info<sycl::info::device::name>()
                    + " - total time spent on calculating:" );

            auto flat_data_base = convert(data_base_);
            std::string_view data = flat_data_base.first;
            std::span<size_t> ids(flat_data_base.second.data(), flat_data_base.second.size());
            sycl::buffer<char, 1> data_buffer(data.data(), data.size());
            sycl::buffer<size_t, 1> ids_buffer(ids.data(), ids.size());

            size_t k = 1;
            auto frequent_items = iter_k1(queue, data_buffer, ids_buffer, k);
            update(frequent_items);
            if(info_logs_) {
                std::cout << k << " frequent_items: " << frequent_items.size() << std::endl;
            }
            k = 2;
            frequent_items = iter_k2(queue, data_buffer, ids_buffer, convert(frequent_items), k);
            update(frequent_items);
            if(info_logs_) {
                std::cout << k << " frequent_items: " << frequent_items.size() << std::endl;
            }
            k = 3;
            while (!frequent_items.empty()) {
                frequent_items = iter_k(queue, data_buffer, ids_buffer, convert(frequent_items), k);
                if(info_logs_) {
                    std::cout << k << " frequent_items: " << frequent_items.size() << std::endl;
                }
                update(frequent_items);
                k++;
            }
        }

    private:
        inline map_items iter_k1(sycl::queue& queue, sycl::buffer<char, 1>& data_buffer, sycl::buffer<size_t, 1>& ids_buffer, size_t k) {
            auto candidates = asyncIterateAndCollect([&](HashNodeSycl& node) {
                return node.iter_1(queue, data_buffer, ids_buffer);
            });
            auto frequentItems = gpu::getFrequentItems(queue, data_buffer, ids_buffer, candidates, min_support_);
            return frequentItems;
        }

        inline map_items iter_k2(sycl::queue& queue, sycl::buffer<char, 1>& data_buffer, sycl::buffer<size_t, 1>& ids_buffer, const std::vector<gsp::item>& frequent_items, size_t k) {
            auto candidates = asyncIterateAndCollect([&](HashNodeSycl& node) {
                return node.iter_2(queue, data_buffer, ids_buffer, frequent_items);
            });
            auto frequentItems = gpu::getFrequentItems(queue, data_buffer, ids_buffer, candidates, min_support_);
            return frequentItems;
        }

        inline map_items iter_k(sycl::queue& queue, sycl::buffer<char, 1>& data_buffer, sycl::buffer<size_t, 1>& ids_buffer, const std::vector<gsp::item>& frequent_items, size_t k) {
            auto flat_frequent_items = convert(frequent_items);
            std::string_view data_frequent_items = flat_frequent_items.first;
            std::span<size_t> ids_frequent_items(flat_frequent_items.second.data(), flat_frequent_items.second.size());
            sycl::buffer<char, 1> data_frequent_items_buffer(data_frequent_items.data(), data_frequent_items.size());
            sycl::buffer<size_t, 1> ids_frequent_items_buffer(ids_frequent_items.data(), ids_frequent_items.size());

            sycl::buffer<std::pair<size_t, size_t>, 1> nums_hashes_buffer(frequent_items.size());

            queue.submit([&](sycl::handler& cgh) {
                sycl::accessor data_frequent_items_access{data_frequent_items_buffer, cgh, sycl::read_only};
                sycl::accessor ids_frequent_items_access{ids_frequent_items_buffer, cgh, sycl::read_only};

                sycl::accessor nums_hashes_accessor{nums_hashes_buffer, cgh, sycl::read_write};

                cgh.parallel_for<class cal_hashes_k>(sycl::range<1>(frequent_items.size()), [=](sycl::id<1> index) {
                    std::string_view frequent_items_gpu_data = std::string_view(data_frequent_items_access.get_pointer(), data_frequent_items_access.size());
                    sycl::span<size_t> frequent_items_gpu_ids(static_cast<size_t*>(ids_frequent_items_access.get_pointer()), ids_frequent_items_access.size());
                    DataBase gpu_frequent_items(frequent_items_gpu_data, frequent_items_gpu_ids);

                    auto transaction = gpu_frequent_items[index];
                    auto sub_element1 = std::string_view(transaction.begin() + 1, transaction.end());
                    auto sub_element2 = std::string_view(transaction.begin(), transaction.begin() + transaction.size() - 1);
                    nums_hashes_accessor[index].first = gpu::getHash(sub_element1, sub_element1.size());
                    nums_hashes_accessor[index].second = gpu::getHash(sub_element2, sub_element2.size());
                });
            });
            sycl::host_accessor nums_hashes_host_accessor{nums_hashes_buffer, sycl::read_only};
            auto candidates = asyncIterateAndCollect([&](HashNodeSycl& node) {
                return node.iter_k(queue, data_buffer, ids_buffer, frequent_items, nums_hashes_host_accessor, k);
            });
            auto frequentItems = gpu::getFrequentItems(queue, data_buffer, ids_buffer, candidates, min_support_);
            return frequentItems;
        }


        template<typename Func>
        std::vector<gsp::item> asyncIterateAndCollect(Func&& f) {
            std::vector<std::future<std::vector<gsp::item>>> futures;
            for (auto& node : nodes_) {
                futures.push_back(std::async(std::launch::async, f, std::ref(node)));
            }

            std::vector<gsp::item> collectedItems;
            for (auto& future : futures) {
                auto fItem = future.get();
                collectedItems.insert(collectedItems.end(), std::make_move_iterator(fItem.begin()), std::make_move_iterator(fItem.end()));
            }

            return collectedItems;
        }

        const size_t num_of_work_group_;
        std::vector<gsp::HashNodeSycl> nodes_;
    };
}