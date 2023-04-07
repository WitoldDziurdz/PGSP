#pragma once
#include <string>
#include <vector>
#include <set>

namespace gsp {

	using item = std::vector<std::string>;
	bool isContain(const std::string& item, const std::string& sub_item);

	bool isSubSequence(const std::vector<std::string>& seq, const std::vector<std::string>& sub_seq);

	std::set<char> getUniqItems(const std::vector<std::string>& sequence);

	std::vector <std::vector<item>> split(const std::vector<item>& vec, size_t base);
	std::set<char> generateUniqItems(const std::vector<gsp::item>& data_base);

	template<typename T>
	std::vector<std::vector<T>> split(const std::vector<T>& input, size_t n) {
		std::vector<std::vector<T>> result;

		for (size_t i = 0; i < input.size(); i += n) {
			std::vector<T> chunk(input.begin() + i, input.begin() + std::min(i + n, input.size()));
			result.push_back(chunk);
		}

		return result;
	}
}