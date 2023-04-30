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

#include "utils.h"
#include "SimpleNode.h"
#include "IEngine.h"

namespace gsp {
    class SPSPMEngineCpu : public IEngine {

    public:
        SPSPMEngineCpu(const std::vector<gsp::item>& data_base, size_t min_support, size_t num_of_work_group);

        void calculate() override;

    private:
        std::map<gsp::item, size_t> calculateFrequentItemsAsync(std::vector<std::vector<gsp::item>>& items);
        const size_t num_of_work_group_;
        std::vector<gsp::SimpleNode> nodes_;
    };
}