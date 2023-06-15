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
        SPSPMEngineGpu(const std::vector<gsp::item>& data_base, size_t min_support, size_t num_of_work_group, bool info_logs = false, bool debug_logs = false):
            IEngine("Simply Partitioned Sequential Pattern Mining GPU", data_base, min_support, info_logs, debug_logs),
            num_of_work_group_{ num_of_work_group } { 

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
            auto candidates = generate_size_1_candidates( data_base_);
            auto frequent_items = gpu::getFrequentItems(queue, data_buffer, ids_buffer, candidates, min_support_);
            if(info_logs_) {
                std::cout << k << " frequent_items: " << frequent_items.size() << std::endl;
            }
            update(frequent_items);

            k = 2;
            candidates = generate_size_2_candidates(frequent_items);
            frequent_items = gpu::getFrequentItems(queue, data_buffer, ids_buffer, candidates,min_support_);
            if(info_logs_) {
                std::cout << k << " frequent_items: " << frequent_items.size() << std::endl;
            }
            update(frequent_items);

            k = 3;
            while (!frequent_items.empty()) {
                candidates = generate_size_k_candidates(frequent_items, k);
                frequent_items = gpu::getFrequentItems(queue, data_buffer, ids_buffer, candidates, min_support_);
                if(info_logs_) {
                    std::cout << k << " frequent_items: " << frequent_items.size() << std::endl;
                }
                update(frequent_items);
                k++;
            }
        }
    private:

        const size_t num_of_work_group_;
    };
}