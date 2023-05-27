#include "SPSPMEngineCpu.h"
#include <vector>
#include <string>
#include <map>
#include "SimpleNode.h"
#include "../core/profile.h"

namespace gsp {

    SPSPMEngineCpu::SPSPMEngineCpu(const std::vector<gsp::item>& data_base, size_t min_support, size_t num_of_work_group) : 
        IEngine("Simply Partitioned Sequential Pattern Mining CPU", data_base, min_support),
        num_of_work_group_{ num_of_work_group } {
        for (size_t i = 0; i < num_of_work_group_; ++i) {
            nodes_.emplace_back(data_base, min_support);
        }
    }

    void SPSPMEngineCpu::calculate() {
        TotalDuration timer(name_ + " - total time speneded on calculating:");
        size_t k = 1;
        auto candidates = generate_size_1_candidates(data_base_);
        auto items = gsp::split(std::move(candidates), nodes_.size());
        auto frequent_items = calculateFrequentItemsAsync(items);
        std::cout << k << " frequent_items: " << frequent_items.size() << std::endl;
        update(frequent_items);

        k = 2;
        candidates = generate_size_2_candidates(frequent_items);
        items = gsp::split(std::move(candidates), nodes_.size());
        frequent_items = calculateFrequentItemsAsync(items);
        std::cout << k << " frequent_items: " << frequent_items.size() << std::endl;
        update(frequent_items);

        k = 3;
        while (!frequent_items.empty()) {
            candidates = generate_size_k_candidates(frequent_items, k);
            items = gsp::split(std::move(candidates), nodes_.size());
            frequent_items = calculateFrequentItemsAsync(items);
            std::cout << k << " frequent_items: " << frequent_items.size() << std::endl;
            update(frequent_items);
            k++;
        }
    }

   map_items SPSPMEngineCpu::calculateFrequentItemsAsync(std::vector<std::vector<gsp::item>>& items) {
        std::vector<std::future<map_items>> futures;
        for (size_t i = 0; i < nodes_.size(); ++i) {
            futures.push_back(std::async(std::launch::async, &SimpleNode::calculateFrequentItems, std::ref(nodes_[i]), std::ref(items[i])));
        }

       map_items frequent_items;
        for (auto& future : futures) {
            auto fr_items = future.get();
            frequent_items.insert(fr_items.begin(), fr_items.end());
        }

        return frequent_items;
    }
}