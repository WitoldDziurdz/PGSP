#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <future>

#include "../core/utils.h"
#include "HashNode.h"
#include "../IEngine.h"

namespace gsp {
	class HashEngineCpu : public IEngine{

	public:
        HashEngineCpu(const std::vector<gsp::item>& data_base, size_t min_support, size_t num_of_work_group, bool info_logs = false, bool debug_logs = false);

        void calculate() override;

    private:
        template<typename Func>
        map_items asyncIterateAndCollect(Func&& f) {
            std::vector<std::future<map_items>> futures;
            for (auto& node : nodes_) {
                futures.push_back(std::async(std::launch::async, f, std::ref(node)));
            }

            map_items collectedItems;
            for (auto& future : futures) {
                auto fItem = future.get();
                collectedItems.insert(std::make_move_iterator(fItem.begin()), std::make_move_iterator(fItem.end()));
            }

            return collectedItems;
        }

        const size_t num_of_work_group_;
        std::vector<gsp::HashNode> nodes_;
	};
}