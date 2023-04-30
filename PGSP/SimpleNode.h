#pragma once
#include <string>
#include <vector>
#include <unordered_map>

#include "utils.h"

namespace gsp {

	class SimpleNode {

	public:

		SimpleNode(const std::vector<gsp::item>& data_base, size_t min_support) : data_base_{ data_base }, min_support_{min_support} {
		}

		map_items calculateFrequentItems(std::vector<gsp::item>& candidates) {
			map_items new_frequent_items = getFrequentItems(data_base_, candidates);
			filter(new_frequent_items, min_support_);
			return new_frequent_items;
		}

	private:

		const std::vector<gsp::item>& data_base_;
		const size_t min_support_;
	};
}