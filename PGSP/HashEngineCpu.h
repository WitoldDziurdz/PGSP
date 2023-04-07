#pragma once
#include <string>
#include <vector>
#include <set>
#include <thread>
#include "utils.h"
#include "Node.h"

namespace gsp {

	class HashEngineCpu {

	public:
		HashEngineCpu(const std::vector<gsp::item>& data_base, size_t num_of_work_group) : num_of_work_group_{ num_of_work_group } {
			auto data_base_chunks = split(data_base, num_of_work_group);
			data_base_pipes_ = std::make_shared<std::vector<gsp::ConcurrentQueue<gsp::item>>>(num_of_work_group);
			for (size_t i = 0; i < num_of_work_group; i++) {
				nodes_.push_back(std::make_unique<gsp::Node>(i, data_base_chunks[i], data_base_pipes_));
			}

			auto items = generateUniqItems(data_base);
			uniq_items.resize(items.size());

			size_t i = 0;
			for (char ch : items) {
				std::string str;
				str += ch;
				uniq_items[i].push_back(str);
				i++;
			}
		}

		void calculate() {
			
		}

	private:
		std::vector<gsp::item> uniq_items;
		size_t num_of_work_group_;
		std::vector<std::unique_ptr<gsp::Node>> nodes_;
		std::shared_ptr<std::vector<gsp::ConcurrentQueue<gsp::item>>> data_base_pipes_;
	};
}