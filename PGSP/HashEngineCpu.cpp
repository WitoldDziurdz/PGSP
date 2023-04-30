#include "HashEngineCpu.h"
#include <vector>
#include <string>
#include <map>

#include "HashNode.h"

namespace gsp {

    HashEngineCpu::HashEngineCpu(const std::vector<gsp::item>& data_base, size_t min_support, size_t num_of_work_group) : IEngine(data_base, min_support), num_of_work_group_{ num_of_work_group } {
        for (size_t i = 0; i < num_of_work_group_; ++i) {
            nodes_.emplace_back(data_base, i, min_support, num_of_work_group);
        }
    }

    void HashEngineCpu::calculate() {
        auto frequentItems = asyncIterateAndCollect([&](HashNode& node) {
            return node.iter_1();
        });

        update(frequentItems);

        auto prevFrequentItems = asyncIterateAndCollect([&](HashNode& node) {
            return node.iter_2(frequentItems);
        });

        update(prevFrequentItems);

        size_t k = 3;
        while (!prevFrequentItems.empty()) {
            frequentItems = std::move(prevFrequentItems);
            prevFrequentItems = asyncIterateAndCollect([&](HashNode& node) {
                return node.iter_k(frequentItems, k);
            });
            update(prevFrequentItems);
            k++;
        }
    }
}