#pragma once
#include <string>
#include <vector>
#include <unordered_map>

#include "../core/utils.h"

namespace gsp {

	class HashNode {

	public:

		HashNode(const std::vector<gsp::item>& data_base, size_t index_node, size_t min_support, size_t max_number_of_nodes);

		map_items iter_1();

		map_items iter_2(const map_items& frequent_items);

		map_items iter_k(const map_items& frequent_items, size_t k);

	private:

		std::vector<gsp::item> generate_size_1_candidates(const std::vector<gsp::item>& database);

		std::vector<gsp::item> generate_size_2_candidates(const map_items& frequent_items);

		std::vector<gsp::item> generate_size_k_candidates(const map_items& frequent_items, size_t k);

        bool isMine(size_t h);
        size_t getHash(ItemIteartor it, size_t k);
		bool isMine(char ch);

		bool isMine(const std::string& str);

		size_t getId(char ch);

		const std::vector<gsp::item>& data_base_;
		const size_t index_node_;
		const size_t min_support_;
		const size_t max_number_of_nodes_;
	};
}