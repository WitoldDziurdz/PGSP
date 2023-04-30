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

	class HashNode {

	public:

		HashNode(const std::vector<gsp::item>& data_base, size_t index_node, size_t min_support, size_t max_number_of_nodes);

		std::map<gsp::item, size_t> iter_1();

		std::map<gsp::item, size_t> iter_2(const std::map<gsp::item, size_t>& frequent_items);

		std::map<gsp::item, size_t> iter_k(const std::map<gsp::item, size_t>& frequent_items, size_t k);

	private:

		std::vector<gsp::item> generate_size_1_candidates(const std::vector<gsp::item>& database);

		std::vector<gsp::item> generate_size_2_candidates(const std::map<gsp::item, size_t>& frequent_items);

		std::vector<gsp::item> generate_size_k_candidates(const std::map<gsp::item, size_t>& frequent_items, size_t k);

		bool isMine(char ch);

		bool isMine(const std::string& str);

		std::vector<size_t> getIds(const std::string& str);

		size_t getId(char ch);

		const std::vector<gsp::item>& data_base_;
		const size_t index_node_;
		const size_t min_support_;
		const size_t max_number_of_nodes_;
	};
}