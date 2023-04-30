#pragma once
#include <string>
#include <vector>
#include <set>
#include <CL/sycl.hpp>

#include "utils.h"

namespace gsp {

	class IEngine {

	public:
		IEngine(const std::vector<gsp::item>& data_base, size_t min_support) : data_base_{ data_base }, min_support_{ min_support } {
		}

		virtual void calculate() = 0;

		std::vector<std::pair<gsp::item, size_t>> getItems() {
			return frequent_items_;
		};

	protected:
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

		void update(const std::map<gsp::item, size_t>& items) {
			for (const auto& el : items) {
				frequent_items_.push_back({ el.first, el.second });
			}
		}

		const std::vector<gsp::item>& data_base_;
		size_t min_support_;
		std::vector<std::pair<gsp::item, size_t>> frequent_items_;
	};
}