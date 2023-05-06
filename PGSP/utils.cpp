#include "utils.h"
#include <algorithm>
#include <unordered_set>
#include <unordered_map>
#include <string_view>

#include "profile.h"

namespace gsp {

	bool isContain(const std::string_view item, const std::string_view sub_item) {
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
		for (auto it = seq.begin(), it_sub = sub_seq.begin(); it != seq.end(); it++) {
			if (isContain(*it, *it_sub)) {
				it_sub++;
			}
			if (it_sub == sub_seq.end()) {
				return true;
			}
		}
		return false;
	}

    std::vector<std::vector<gsp::item>> split(std::vector<gsp::item> items, size_t num_of_work_group) {
        if (num_of_work_group == 0) {
            throw std::invalid_argument("Number of work groups must be greater than 0");
        }

        std::vector<std::vector<gsp::item>> result(num_of_work_group);

        for (size_t i = 0; i < items.size(); ++i) {
            size_t group_index = i % num_of_work_group;
            result[group_index].push_back(std::move(items[i]));
        }

        return result;
    }

    std::vector<gsp::item> generate_size_1_candidates(const std::vector<gsp::item>& database) {
        std::set<gsp::item> candidates;
        for (const auto& transaction : database) {
            for (const auto& element : transaction) {
                for (const auto& ev : element) {
                    std::string str;
                    str += ev;
                    gsp::item seq = { str };
                    candidates.insert(seq);
                }
            }
        }
        return { candidates.begin(), candidates.end() };
    }

    std::vector<gsp::item> generate_size_2_candidates(const map_items& frequent_items) {
        std::set<gsp::item> candidates;
        for (const auto& pr1 : frequent_items) {
            const std::string& element1 = *pr1.first.begin();
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
        return { candidates.begin(), candidates.end() };
    }

    std::string flatItem(const gsp::item& element) {
        if (element.empty()) {
            return {};
        }

        std::string result;

        size_t totalLength = 0;
        for (const auto& el : element) {
            totalLength += el.length();
        }

        result.reserve(totalLength);

        for (const auto& el : element) {
            result += el;
        }

        return result;
    }

    bool isCanBeCandidate(const gsp::item& first, const gsp::item& second) {
        auto len1 = std::accumulate(first.begin(), first.end(), 0, [](size_t acc, const auto& s) {
            return acc + s.size();
        });
        auto len2 = std::accumulate(second.begin(), second.end(), 0, [](size_t acc, const auto& s) {
            return acc + s.size();
        });

        if (len1 != len2) {
            return false;
        }

        ItemIteartor it1(first);
        ItemIteartor it2(second);
        ++it1;
        for (size_t i = 1; i < len1; ++i) {
            if ((*it1) != (*it2)) {
                return false;
            }
            ++it1;
            ++it2;
        }

        return true;
    }

    bool needMerge(const gsp::item& element1, const gsp::item& element2) {
        if (element1.back().size() > 1 || element2.back().size() > 1) {
            return true;
        }
        return false;
    }

    size_t getSize(const gsp::item& item) {
        size_t sum = std::accumulate(item.begin(), item.end(), 0, [](size_t s, const auto& e) {
            return s + e.size();
        });
        return sum;
    }

    std::vector<gsp::item> generate_size_k_candidates(const map_items& frequent_items, size_t k) {
        std::set<gsp::item> candidates;
        for (const auto& pr1 : frequent_items) {
            const auto& element1 = pr1.first;
             for (const auto& pr2 : frequent_items) {
                    const auto& element2 = pr2.first;
                    if (isCanBeCandidate(element1, element2)) {
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
        return { candidates.begin(), candidates.end() };
    }

	void filter(map_items& frequency, size_t min_support) {
		std::erase_if(frequency, [min_support](const auto& item) {
			auto const& [key, value] = item;
			return value < min_support;
		});
	}

    map_items getFrequentItems(const std::vector<gsp::item>& data_base, std::vector<item>& candidates) {
        map_items frequent_elements;
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

    bool isCanBeFrequent(const std::set<std::string>& frequent_items, const gsp::item& candidate) {
        const auto flat_item = flatItem(candidate);
        for (size_t i = 0; i < flat_item.size(); ++i) {
            std::string str = flat_item;
            str.erase(i, 1);
            if (!frequent_items.count(str)) {
                return false;
            }
        }
        return true;
    }

    std::vector<gsp::item> prune(const map_items& frequent_items, const std::vector <gsp::item>& candidates) {
        std::vector<gsp::item> results;
        std::set<std::string> flat_frequent_items;
        for (const auto& item : frequent_items) {
            flat_frequent_items.insert(flatItem(item.first));
        }

        for (const auto& candidate : candidates) {
            if (isCanBeFrequent(flat_frequent_items, candidate)) {
                results.push_back(candidate);
            }
        }
        return results;
    }

    std::ostream& operator<<(std::ostream& os, const item& item) {
        os << "[";
        bool isFirst = true;
        for (const auto& ev : item) {
            if (!isFirst) {
                os << ", ";
            }
            isFirst = false;
            os << ev;
        }
        os << "]";
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const std::pair<gsp::item, size_t>& item) {
        os << "{";
        os << item.first << " : " << item.second;
        os << "}";
        return os;
    }
    
    std::ostream& operator<<(std::ostream& os, const std::vector<std::pair<gsp::item, size_t>>& items) {
        for (const auto& item : items) {
            os << item << std::endl;
        }
        return os;
    }

    std::ostream& operator<<(std::ostream& os, const std::map<gsp::item, size_t>& items) {
        for (const auto& item : items) {
            os << item << std::endl;
        }
        return os;
    }


    void print(const std::vector<std::pair<gsp::item, size_t>>& items) {
        std::map<gsp::item, size_t> elements(items.begin(), items.end());
        std::cout << "-------------------------" << std::endl;
        std::cout << elements;
        std::cout << "-------------------------" << std::endl;
    }

    void writeToFile(const std::vector<std::pair<gsp::item, size_t>>& items, std::string fileName) {
        std::ofstream myfile(fileName);

        if (myfile.is_open()) {
            myfile << items;

            myfile.close();
        }
        else {
            std::cerr << "Error: Unable to open the file for writing." << std::endl;
        }
    }
}