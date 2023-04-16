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
		GspEngineCpu(const std::vector<gsp::item>& data_base, size_t min_support, size_t num_of_work_group) : data_base_{ data_base }, min_support_{ min_support }, num_of_work_group_{ num_of_work_group } {
		}

		void calculate() {

			auto items1 = generate_size_1_candidates(data_base_);
			auto frequent_items1 = getFrequentItems(data_base_, items1);
			print(frequent_items1);
			filter(frequent_items1, 2);
			print(frequent_items1);
			update(frequent_items1);

			auto items = generate_size_2_candidates(frequent_items1);
			auto frequent_items = getFrequentItems(data_base_, items);
			print(frequent_items);
			filter(frequent_items, 2);
			print(frequent_items);
			update(frequent_items);

			size_t k = 3;
			while (!frequent_items.empty()) {
				items = generate_size_k_candidates(frequent_items, k);
				print(items);
				frequent_items = getFrequentItems(data_base_, items);
				print(frequent_items);
				filter(frequent_items, 2);
				print(frequent_items);
				update(frequent_items);
				k++;
			}
		}

		std::vector<std::pair<gsp::item, size_t>> getItems() {
			return frequent_items_;
		}

	private:

		void update(const std::map<gsp::item, size_t>& items) {
			for (const auto& el : items) {
				frequent_items_.push_back({ el.first, el.second});
			}
		}

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
		std::vector<std::pair<gsp::item, size_t>> frequent_items_;
	};
}