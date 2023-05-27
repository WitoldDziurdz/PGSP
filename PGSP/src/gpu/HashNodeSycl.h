#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <sycl/sycl.hpp>

#include "../core/utils.h"
#include "../core/sycl_utiils.h"

namespace gsp {

	class HashNodeSycl {

	public:

        HashNodeSycl(const std::vector<gsp::item>& data_base, size_t index_node, size_t min_support, size_t max_number_of_nodes) :
                data_base_{ data_base },
                index_node_{ index_node },
                min_support_{ min_support },
                max_number_of_nodes_{ max_number_of_nodes } {
        }

        map_items iter_1( sycl::queue& queue,
                          sycl::buffer<char, 1>& data_buffer,
                          sycl::buffer<size_t, 1>& ids_buffer) {
            auto candidates = generate_size_1_candidates(data_base_);
            auto frequent_items = gpu::getFrequentItems(queue, data_buffer, ids_buffer, candidates, min_support_);
            filter(frequent_items, min_support_);
            return frequent_items;
        }

		map_items iter_2(sycl::queue& queue,
                         sycl::buffer<char, 1>& data_buffer,
                         sycl::buffer<size_t, 1>& ids_buffer,
                         const std::vector<gsp::item>& frequent_items){
            auto candidates = generate_size_2_candidates(frequent_items);
            auto items = gpu::getFrequentItems(queue, data_buffer, ids_buffer, candidates, min_support_);
            filter(items, min_support_);
            return items;
        }

        map_items iter_k(sycl::queue& queue,
                         sycl::buffer<char, 1>& data_buffer,
                         sycl::buffer<size_t, 1>& ids_buffer,
                         const std::vector<gsp::item>& frequent_items,
                         sycl::host_accessor<std::pair<size_t, size_t>, 1, sycl::access::mode::read>& buffer_accessor, size_t k){
            auto candidates = generate_size_k_candidates(frequent_items, buffer_accessor, k);
            auto items = gpu::getFrequentItems(queue, data_buffer, ids_buffer, candidates, min_support_);
            filter(items, min_support_);
            return items;
        }

	private:

        std::vector<gsp::item> generate_size_1_candidates(const std::vector<gsp::item>& database) {
            std::set<gsp::item> candidates;
            for (const auto& transaction : database){
                for (const auto& element : transaction) {
                    for (const auto& ev : element) {
                        if (isMine(static_cast<size_t>(ev))) {
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

        std::vector<gsp::item> generate_size_2_candidates(const std::vector<gsp::item>& frequent_items) {
            std::set<gsp::item> candidates;
            for (size_t i = 0; i < frequent_items.size(); ++i) {
                const std::string& element1 = *frequent_items[i].begin();
                if (isMine(static_cast<size_t>(element1[0]))) {
                    for (size_t j = 0; j < frequent_items.size(); ++j) {
                        const std::string& element2 = *frequent_items[j].begin();
                        if (element1 != element2) {
                            auto str = element1 + element2;
                            std::sort(str.begin(), str.end());
                            candidates.insert({ str });
                        }
                        candidates.insert({ element1, element2 });
                    }
                }
            }
            return { candidates.begin(), candidates.end() };
        }

        std::vector<gsp::item> generate_size_k_candidates(const std::vector<gsp::item>& frequent_items,
                                                          sycl::host_accessor<std::pair<size_t, size_t>, 1, sycl::access::mode::read>& buffer_accessor,
                                                          size_t k) {
            std::set<gsp::item> candidates;
            size_t len = k-1;
            for (size_t i = 0; i < frequent_items.size(); ++i) {
                const auto& element1 = frequent_items[i];
                auto h1 = buffer_accessor[i].first;
                if (isMine(h1)) {
                    for (size_t j = 0; j < frequent_items.size(); ++j) {
                        const auto& element2 = frequent_items[j];
                        auto h2 = buffer_accessor[j].second;
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

        bool isMine(size_t h) {
            return (h % max_number_of_nodes_) == index_node_;
        }

		const std::vector<gsp::item>& data_base_;
		const size_t index_node_;
		const size_t min_support_;
		const size_t max_number_of_nodes_;
	};
}

