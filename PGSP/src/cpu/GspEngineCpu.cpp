#include <string>
#include <vector>
#include <map>

#include "../core/utils.h"
#include "HashNode.h"
#include "GspEngineCpu.h"
#include "../core/profile.h"

namespace gsp {

	GspEngineCpu::GspEngineCpu(const std::vector<gsp::item>& data_base, size_t min_support) : 
		IEngine("Generalized Sequential Pattern", data_base, min_support) {
	}

	void GspEngineCpu::calculate() {
		TotalDuration timer(name_ + " - total time speneded on calculating:");
		auto items = generate_size_1_candidates(data_base_);
		auto frequent_items = getFrequentItems(data_base_, items);
		filter(frequent_items, min_support_);
		update(frequent_items);

		items = generate_size_2_candidates(frequent_items);
		items = prune(frequent_items, items);
		frequent_items = getFrequentItems(data_base_, items);
		filter(frequent_items, min_support_);
		update(frequent_items);

		size_t k = 3;
		while (!frequent_items.empty()) {
			items = generate_size_k_candidates(frequent_items, k);
            std::cout << k << " canditates: " << items.size() << std::endl;
			items = prune(frequent_items, items);
			frequent_items = getFrequentItems(data_base_, items);
			filter(frequent_items, min_support_);
			update(frequent_items);
			k++;
		}
	}
}