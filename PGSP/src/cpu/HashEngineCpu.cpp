#include "HashEngineCpu.h"
#include <vector>
#include <string>
#include <map>

#include "HashNode.h"
#include "../core/profile.h"

namespace gsp {

    HashEngineCpu::HashEngineCpu(const std::vector<gsp::item>& data_base, size_t min_support, size_t num_of_work_group, bool info_logs, bool debug_logs) :
        IEngine("Hash Partitioned Sequential Pattern Mining CPU", data_base, min_support, info_logs, debug_logs),
        num_of_work_group_{num_of_work_group} {
        for (size_t i = 0; i < num_of_work_group_; ++i) {
            nodes_.emplace_back(data_base, i, min_support, num_of_work_group);
        }
    }

    void HashEngineCpu::calculate() {
        TotalDuration timer(name_ + " AMD Ryzen 5 3600X 6-Core Processor - total time spent on calculating:");
        size_t k = 1;
        auto frequentItems = asyncIterateAndCollect([&](HashNode& node) {
            return node.iter_1();
        });
        if(info_logs_) {
            std::cout << k << " frequent_items: " << frequentItems.size() << std::endl;
        }
        update(frequentItems);

        k = 2;
        auto prevFrequentItems = asyncIterateAndCollect([&](HashNode& node) {
            return node.iter_2(frequentItems);
        });
        if(info_logs_) {
            std::cout << k << " frequent_items: " << prevFrequentItems.size() << std::endl;
        }
        update(prevFrequentItems);

        k = 3;
        while (!prevFrequentItems.empty()) {
            frequentItems = std::move(prevFrequentItems);
            prevFrequentItems = asyncIterateAndCollect([&](HashNode& node) {
                return node.iter_k(frequentItems, k);
            });
            if(info_logs_) {
                std::cout << k << " frequent_items: " << prevFrequentItems.size() << std::endl;
            }
            update(prevFrequentItems);
            k++;
        }
    }
}