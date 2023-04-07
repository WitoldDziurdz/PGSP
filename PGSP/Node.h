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

		Node(size_t index_node, const std::vector<gsp::item> data_base, 
			std::shared_ptr<std::vector<gsp::ConcurrentQueue<gsp::item>>> data_base_pipes) :
			index_node_{index_node},
			dataBase_(data_base), 
			data_base_pipes_(data_base_pipes){
			reset();
		}

		void readAndCount(size_t sizeOfSequence) {
			while (not wasReaded()) {
				read(1, sizeOfSequence);
				count();
			}
		}

		void count() {
			gsp::ConcurrentQueue<gsp::item>& items = data_base_pipes_->at(index_node_);
			while (!items.empty())
			{
				auto el = items.try_pop();
				if (el) {
					chechCandidates(*el);
				}
			}
		}

		void read(size_t nums, size_t sizeOfSequence) {
			while ((not wasReaded()) && (nums > 0)) {
				auto elements = dataBase_.read(sizeOfSequence);
				for (size_t i = 0; i < elements.size(); ++i) {
					setElement(std::move(elements[i]));
				}
				nums--;
			}
		}

		void generate(std::map<gsp::item, size_t>& frequent_items) {
			if (not frequent_items.empty()) {
				std::vector<gsp::item> frequent_elements;
				frequent_elements.reserve(frequent_items.size());
				std::for_each(frequent_items.begin(), frequent_items.end(), [&frequent_elements](auto& item) {
					frequent_elements.push_back(item.first);
				});
				generateCandidates(frequent_elements);
			}
		}

		std::map<gsp::item, size_t> getFrequentItems(size_t min_support) {
			const auto count = std::erase_if(candidates_, [min_support](const auto& item) {
				auto const& [key, value] = item;
				return value < min_support;
			});
			return candidates_;
		}


		bool wasReaded() {
			return dataBase_.wasReaded();
		}

		void reset() {
			dataBase_.reset();
			candidates_.clear();
		}


	private:

		void chechCandidates(const gsp::item& item) {
			for (std::pair<const gsp::item, size_t>& candidate : candidates_) {
				if (gsp::isSubSequence(item, candidate.first)) {
					candidate.second++;
				}
			}
		}

		void generateCandidates(const std::vector<gsp::item>& frequent_elements) {
			for (const auto& seq1 : frequent_elements) {
				for (const auto& seq2 : frequent_elements) {
					if (seq1.size() == seq2.size()) {
						bool canJoin = true;

						for (size_t i = 0; i < seq1.size() - 1; ++i) {
							if (seq1[i] != seq2[i]) {
								canJoin = false;
								break;
							}
						}

						if (canJoin && seq1.back() != seq2.back() && isMine(seq1[0])) {
							gsp::item newCandidate = seq1;
							newCandidate.push_back(seq2.back());
							candidates_.insert({ newCandidate, 0 });
						}
					}
				}
			}
		}


		void setElement(std::unique_ptr<gsp::item> element) {
			auto ids= getIds(element->at(0));
			if (ids.size() == 1) {
				data_base_pipes_->at(ids[0]).push(std::move(element));
			}
			else {
				for (auto id : ids) {
					auto el = std::make_unique<gsp::item>(*element);
					data_base_pipes_->at(id).push(std::move(el));
				}
			}
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
			return std::hash<char>{}(ch) % data_base_pipes_->size();
		}


		size_t index_node_;
		DataBase dataBase_;
		std::shared_ptr<std::vector<gsp::ConcurrentQueue<gsp::item>>> data_base_pipes_;
		std::map<gsp::item, size_t> candidates_;
	};
}