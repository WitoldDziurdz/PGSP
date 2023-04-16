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

	class HashEngineCpu {

	public:
		HashEngineCpu(const std::vector<gsp::item>& data_base, size_t min_support, size_t num_of_work_group) : data_base_{ data_base }, min_support_{min_support}, num_of_work_group_ { num_of_work_group } {
			auto data_base_chunks = split(data_base, num_of_work_group);
			data_base_pipes_ = std::make_shared<std::vector<gsp::ConcurrentQueue<gsp::item>>>(num_of_work_group);
			for (size_t i = 0; i < num_of_work_group; i++) {
				nodes_.push_back(std::make_unique<gsp::Node>(i, data_base_chunks[i], data_base_pipes_));
			}
		}

		void calculate() {

			auto items1 = generate_size_1_candidates(data_base_);
			auto frequent_items1 = getFrequentItems(data_base_, items1);
			print(frequent_items1);
			filter(frequent_items1, 2);
			print(frequent_items1);

			auto items = generate_size_2_candidates(frequent_items1);
			auto frequent_items = getFrequentItems(data_base_, items);
			print(frequent_items);
			filter(frequent_items, 2);
			print(frequent_items);
			size_t k = 3;
			while (!frequent_items.empty()) {
				items = generate_size_k_candidates(frequent_items, k);
				print(items);
				frequent_items = getFrequentItems(data_base_, items);
				print(frequent_items);
				filter(frequent_items, 2);
				print(frequent_items);
				k++;
			}
		}

	private:

		void print(const std::vector<gsp::item>& items) {
			std::cout << "+++++++++++++++++++++++++" << std::endl;
			for (const auto& item : items) {
				for (const auto& ev : item) {
					std::cout << ev << " ";
				}
				std::cout << std::endl;
			}
			std::cout << "+++++++++++++++++++++++++" << std::endl;
		}

		void print(const std::map<gsp::item, size_t>& items) {
			std::cout << "-------------------------" << std::endl;
			for (const auto& item : items) {
				for (const auto& ev : item.first) {
					std::cout << ev << " ";
				}
				std::cout << item.second << std::endl;
			}
			std::cout << "-------------------------" << std::endl;
		}

		const std::vector<gsp::item>& data_base_;
		size_t min_support_;
		size_t num_of_work_group_;
		std::vector<std::unique_ptr<gsp::Node>> nodes_;
		std::shared_ptr<std::vector<gsp::ConcurrentQueue<gsp::item>>> data_base_pipes_;
	};
}