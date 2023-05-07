#include "HashEngineCpu.h"
#include <vector>
#include <string>
#include <map>

#include "HashNode.h"

namespace gsp {
	HashNode::HashNode(const std::vector<gsp::item>& data_base, size_t index_node, size_t min_support, size_t max_number_of_nodes) :
		data_base_{ data_base },
		index_node_{ index_node },
		min_support_{ min_support },
		max_number_of_nodes_{ max_number_of_nodes } {
	}

	map_items HashNode::iter_1() {
		auto items = generate_size_1_candidates(data_base_);
		auto frequent_items = getFrequentItems(data_base_, items);
		filter(frequent_items, min_support_);
		return frequent_items;
	}

	map_items HashNode::iter_2(const map_items& frequent_items) {
		auto items = generate_size_2_candidates(frequent_items);
		map_items new_frequent_items = getFrequentItems(data_base_, items);
		filter(new_frequent_items, min_support_);
		return new_frequent_items;
	}

	map_items HashNode::iter_k(const map_items& frequent_items, size_t k) {
		auto items = generate_size_k_candidates(frequent_items, k);
		map_items new_frequent_items = getFrequentItems(data_base_, items);
		filter(new_frequent_items, min_support_);
		return new_frequent_items;
	}

	std::vector<gsp::item> HashNode::generate_size_1_candidates(const std::vector<gsp::item>& database) {
		std::set<gsp::item> candidates;
		for (const auto& transaction : database) {
            ItemIteartor it(transaction);
            auto h = getHash(it, transaction.size());
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

	std::vector<gsp::item> HashNode::generate_size_2_candidates(const map_items& frequent_items) {
		std::set<gsp::item> candidates;
		for (const auto& pr1 : frequent_items) {
			const std::string& element1 = *pr1.first.begin();
			if (isMine(element1)) {
				for (const auto& pr2 : frequent_items) {
					const std::string& element2 = *pr2.first.begin();
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

	std::vector<gsp::item> HashNode::generate_size_k_candidates(const map_items& frequent_items, size_t k) {
		std::set<gsp::item> candidates;
        size_t len = k-1;
		for (const auto& pr1 : frequent_items) {
			const auto& element1 = pr1.first;
            ItemIteartor iterator1(element1);
            iterator1++;
            auto h1 = getHash(iterator1, len-1);
			if (isMine(h1)) {
				for (const auto& pr2 : frequent_items) {
					const auto& element2 = pr2.first;
                    auto h2 = getHash(element2, len-1);
					if ((h1 == h2) && isCanBeCandidate(element1, element2, len)) {
						gsp::item candidate = element1;
						auto pr = element2.back().back();
						if (needMerge(element1, element2)) {
							candidate.back() += pr;
							std::sort(candidate.back().begin(), candidate.back().end());
							if (getSize(candidate) == k) {
								candidates.insert(candidate);
							}
						}
						gsp::item candidate2 = element1;
						std::string str;
						str += pr;
						candidate2.push_back(str);
						if (getSize(candidate2) == k) {
							candidates.insert(candidate2);
						}
					}
				}
			}
		}
		return { candidates.begin(), candidates.end() };
	}

    bool HashNode::isMine(size_t h) {
        return (h % max_number_of_nodes_) == index_node_;
    }

    size_t HashNode::getHash(ItemIteartor it, size_t k) {
        size_t sum = 0;
        constexpr size_t offset = 8;
        for (size_t i = 0; i < k; ++i) {
            sum += (static_cast<size_t>(*it) << offset);
            ++it;
        }
        return sum;
    }

	bool HashNode::isMine(char ch) {
		return getId(ch) == index_node_;
	}

	bool HashNode::isMine(const std::string& str) {
		return isMine(str[0]);
	}

	size_t HashNode::getId(char ch) {
		return static_cast<size_t>(ch) % max_number_of_nodes_;
	}
}