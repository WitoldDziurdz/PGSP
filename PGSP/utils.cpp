#include "utils.h"
#include  <algorithm>
#include <unordered_set>

namespace gsp {
	template<typename T>
	size_t getIndex(T item, size_t base) {
		return std::hash(item) % base;
	}

	bool isContain(const std::string& item, const std::string& sub_item) {
		for (char c : sub_item) {
			if (std::count(item.begin(), item.end(), c) < std::count(sub_item.begin(), sub_item.end(), c)) {
				return false;
			}
		}
		return true;
	}

	bool isSubSequence(const std::vector<std::string>& seq, const std::vector<std::string>& sub_seq) {
		if (seq.size() < sub_seq.size()) {
			return false;
		}
		for (auto it = seq.begin(), it_sub = sub_seq.begin();
			it != seq.end(); it++) {
			if (isContain(*it, *it_sub)) {
				it_sub++;
			}
			if (it_sub == sub_seq.end()) {
				return true;
			}
		}
		return false;
	}

	std::set<char> gsp::getUniqItems(const std::vector<std::string>& sequence) {
		std::set<char> result;
		for (const std::string& item : sequence) {
			result.insert(item.begin(), item.end());
		}
		return result;
	}

	std::vector <std::vector<item>> split(const std::vector<item>& vec, size_t base) {
		std::vector<std::vector<item>> outVec;

		size_t length = vec.size() / base;
		size_t remain = vec.size() % base;

		size_t begin = 0;
		size_t end = 0;

		for (size_t i = 0; i < std::min(base, vec.size()); ++i)
		{
			end += (remain > 0) ? (length + !!(remain--)) : length;

			outVec.push_back(std::vector<item>(vec.begin() + begin, vec.begin() + end));

			begin = end;
		}

		return outVec;
	}

	std::set<char> generateUniqItems(const std::vector<gsp::item>& data_base) {
		std::set<char> uniq_items;
		for (const auto& items : data_base) {
			for (const auto& item : items) {
				uniq_items.insert(item.cbegin(), item.cend());
			}
		}
		return uniq_items;
	}

	void filter(std::map<gsp::item, size_t>& frequency, size_t min_support) {
		std::erase_if(frequency, [min_support](const auto& item) {
			auto const& [key, value] = item;
			return value < min_support;
		});
	}

	std::map<gsp::item, size_t> getFrequentItems(const std::vector<gsp::item>& data_base, std::vector<item>& candidates) {
		std::map<gsp::item, size_t> frequent_elements;
		if (candidates.empty()) {
			return {};
		}
		size_t k = candidates.begin()->size();
		for (const item& sub_seq : candidates) {
			for (const item& seq : data_base) {
				if (gsp::isSubSequence(seq, sub_seq)) {
					++frequent_elements[sub_seq];
				}
			}
		}
		return frequent_elements;
	}
}