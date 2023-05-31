#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>
#include <thread>
#include <memory>
#include <iostream>
#include <algorithm>
#include <future>

#include "../core/utils.h"
#include "SimpleNode.h"
#include "../IEngine.h"

namespace gsp {
    class SPSPMEngineCpu : public IEngine {

    public:
        SPSPMEngineCpu(const std::vector<gsp::item>& data_base, size_t min_support, size_t num_of_work_group, bool info_logs = false, bool debug_logs = false);

        void calculate() override;
    private:
        map_items calculateFrequentItemsAsync(std::vector<std::vector<gsp::item>>& items);
        const size_t num_of_work_group_;
        std::vector<gsp::SimpleNode> nodes_;
    };
}