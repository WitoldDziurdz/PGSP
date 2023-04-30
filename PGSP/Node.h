#pragma once
#include <string>
#include <vector>
#include <set>
#include <numeric>
#include <thread>
#include <memory>
#include <map>
#include <algorithm>

#include "queue.h"
#include "utils.h"
#include "DataBase.h"

namespace gsp {

	class Node {

	public:
		Node(const Node&) = delete;
		Node& operator=(const Node&) = delete;

		Node(const std::vector<gsp::item>& data_base, size_t index_node, size_t min_support, size_t max_number_of_nodes) :
			data_base_{ data_base},
			index_node_{index_node},
			min_support_{ min_support}, 
			max_number_of_nodes_{ max_number_of_nodes } {
		}

		std::map<gsp::item, size_t> iter_1() {
			auto items = generate_size_1_candidates(data_base_);
			auto frequent_items = getFrequentItems(data_base_, items);
			filter(frequent_items, min_support_);
			return frequent_items;
		}

		std::map<gsp::item, size_t> iter_2(const std::map<gsp::item, size_t>& frequent_items) {
			auto items = generate_size_2_candidates(frequent_items);
			std::map<gsp::item, size_t> new_frequent_items = getFrequentItems(data_base_, items);
			filter(new_frequent_items, min_support_);
			return new_frequent_items;
		}

		std::map<gsp::item, size_t> iter_k(const std::map<gsp::item, size_t>& frequent_items) {
			auto items = generate_size_2_candidates(frequent_items);
			std::map<gsp::item, size_t> new_frequent_items = getFrequentItems(data_base_, items);
			filter(new_frequent_items, min_support_);
			return new_frequent_items;
		}


	private:

		std::vector<gsp::item> generate_size_1_candidates(const std::vector<gsp::item>& database) {
			std::set<gsp::item> candidates;
			for (const auto& transaction : database) {
				for (const auto& element : transaction) {
					for (const auto& ev : element) {
						if (isMine(ev)) {
							std::string str;
							str += ev;
							gsp::item seq = { str };
							candidates.insert(seq);
						}
					}
				}
			}
			return { candidates.begin(), candidates.end() };
		}

		std::vector<gsp::item> generate_size_2_candidates(const std::map<gsp::item, size_t>& frequent_items) {
			std::set<gsp::item> candidates;
			for (const auto& pr1 : frequent_items) {
				const std::string element1 = *pr1.first.begin();
				if (isMine(element1)) {
					for (const auto& pr2 : frequent_items) {
						const std::string element2 = *pr2.first.begin();
						if (element1 != element2) {
							auto str = element1 + element2;
							std::sort(str.begin(), str.end());
							candidates.insert({ str });
						}
						candidates.insert({ element1,  element2 });
					}
				}
			}
			return { candidates.begin(), candidates.end() };
		}

		std::vector<gsp::item> generate_size_k_candidates(const std::map<gsp::item, size_t>& frequent_items, size_t k) {
			std::set<gsp::item> candidates;

			for (const auto& pr1 : frequent_items) {
				for (const auto& pr2 : frequent_items) {
					const auto& element1 = pr1.first;
					const auto& element2 = pr2.first;
					if (isMine(*element1.begin())) {
						auto new_element1 = deleteFirstElement(element1);
						auto new_element2 = deleteLastElement(element2);
						if (new_element1 == new_element2) {
							auto pr = getLastElement(element2);
							gsp::item candidate = element1;
							if (needMerge(element1, element2)) {
								candidate.back() += pr;
								std::sort(candidate.back().begin(), candidate.back().end());
							}
							gsp::item candidate2 = element1;
							candidate2.push_back(pr);
							if (getSize(candidate) == k) {
								candidates.insert(candidate);
							}
							if (getSize(candidate2) == k) {
								candidates.insert(candidate2);
							}
						}
					}
				}
			}
			return { candidates.begin(), candidates.end() };
		}

		bool isMine(char ch) {
			return getId(ch) == index_node_;
		}

		bool isMine(const std::string& str) {
			auto ids = getIds(str);
			return std::find(ids.cbegin(), ids.cend(), index_node_) != ids.cend();
		}

		std::vector<size_t> getIds(const std::string& str) {
			std::vector<size_t> ids;
			for (char ch : str) {
				ids.push_back(getId(ch));
			}
			return ids;
		}

		size_t getId(char ch) {
			return std::hash<char>{}(ch) % max_number_of_nodes_;
		}

		const std::vector<gsp::item>& data_base_;
		const size_t index_node_;
		const size_t min_support_;
		const size_t max_number_of_nodes_;
	};
}