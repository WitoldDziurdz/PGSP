#pragma once
#include <string>
#include <vector>
#include <map>
#include <future>

#include "utils.h"
#include "HashNode.h"
#include "IEngine.h"

namespace gsp {
	class HashEngineCpu : public IEngine{

	public:
        HashEngineCpu(const std::vector<gsp::item>& data_base, size_t min_support, size_t num_of_work_group);

        void calculate() override;

    private:
        template<typename Func>
        std::map<gsp::item, size_t> asyncIterateAndCollect(Func&& f) {
            std::vector<std::future<std::map<gsp::item, size_t>>> futures;
            for (auto& node : nodes_) {
                futures.push_back(std::async(std::launch::async, f, std::ref(node)));
            }

            std::map<gsp::item, size_t> collectedItems;
            for (auto& future : futures) {
                auto fItem = future.get();
                collectedItems.insert(fItem.begin(), fItem.end());
            }

            return collectedItems;
        }

        const size_t num_of_work_group_;
        std::vector<gsp::HashNode> nodes_;
	};
}