#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>
#include <thread>
#include "utils.h"
#include "Node.h"
#include <iostream>

namespace gsp {

	class GspEngineCpu {

	public:
		GspEngineCpu(const std::vector<gsp::item>& data_base, size_t min_support, size_t num_of_work_group);

		void calculate();

		std::vector<std::pair<gsp::item, size_t>> getItems();

	private:

		void update(const std::map<gsp::item, size_t>& items);
		const std::vector<gsp::item>& data_base_;
		size_t min_support_;
		std::vector<std::pair<gsp::item, size_t>> frequent_items_;
	};
}