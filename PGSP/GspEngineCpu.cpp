#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>
#include <thread>
#include "utils.h"
#include "Node.h"
#include <iostream>

#include "GspEngineCpu.h"

namespace gsp {

	GspEngineCpu::GspEngineCpu(const std::vector<gsp::item>& data_base, size_t min_support, size_t num_of_work_group) : data_base_{ data_base }, min_support_{ min_support } {
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

	std::vector<std::pair<gsp::item, size_t>> GspEngineCpu::getItems() {
		return frequent_items_;
	}

	void GspEngineCpu::update(const std::map<gsp::item, size_t>& items) {
		for (const auto& el : items) {
			frequent_items_.push_back({ el.first, el.second });
		}
	}
}