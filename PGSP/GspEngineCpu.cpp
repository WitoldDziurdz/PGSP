#include <string>
#include <vector>
#include <map>
#include <set>
#include <thread>
#include "utils.h"
#include "HashNode.h"
#include <iostream>

#include "GspEngineCpu.h"

namespace gsp {

	GspEngineCpu::GspEngineCpu(const std::vector<gsp::item>& data_base, size_t min_support) : IEngine(data_base, min_support) {
	}

	void GspEngineCpu::calculate() {
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
			items = prune(frequent_items, items);
			frequent_items = getFrequentItems(data_base_, items);
			filter(frequent_items, min_support_);
			update(frequent_items);
			k++;
		}
	}
}