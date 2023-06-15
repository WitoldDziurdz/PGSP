#include <string>
#include <vector>
#include <map>

#include "../core/utils.h"
#include "HashNode.h"
#include "GspEngineCpu.h"
#include "../core/profile.h"

namespace gsp {

	GspEngineCpu::GspEngineCpu(const std::vector<gsp::item>& data_base, size_t min_support, bool info_logs, bool debug_logs) :
		IEngine("Generalized Sequential Pattern", data_base, min_support, info_logs, debug_logs) {
	}

	void GspEngineCpu::calculate() {
		TotalDuration timer(name_ + " AMD Ryzen 5 3600X 6-Core Processor - total time spent on calculating:");
        size_t k = 1;
		auto items = generate_size_1_candidates(data_base_);
		auto frequent_items = getFrequentItems(data_base_, items);
		filter(frequent_items, min_support_);
        if(info_logs_) {
            std::cout << k << " frequent_items: " << frequent_items.size() << std::endl;
        }
		update(frequent_items);

        k = 2;
		items = generate_size_2_candidates(frequent_items);
		items = prune(frequent_items, items);
		frequent_items = getFrequentItems(data_base_, items);
		filter(frequent_items, min_support_);
        if(info_logs_) {
            std::cout << k << " frequent_items: " << frequent_items.size() << std::endl;
        }
		update(frequent_items);

		k = 3;
		while (!frequent_items.empty()) {
			items = generate_size_k_candidates(frequent_items, k);
			items = prune(frequent_items, items);
			frequent_items = getFrequentItems(data_base_, items);
			filter(frequent_items, min_support_);
            if(info_logs_) {
                std::cout << k << " frequent_items: " << frequent_items.size() << std::endl;
            }
			update(frequent_items);
			k++;
		}
	}
}