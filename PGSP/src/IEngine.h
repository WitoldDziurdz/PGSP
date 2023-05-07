#pragma once
#include <string>
#include <vector>
#include <unordered_map>

#include "core/utils.h"

namespace gsp {

	class IEngine {

	public:
		IEngine(const std::string& name, const std::vector<gsp::item>& data_base, size_t min_support) : name_{name}, data_base_ { data_base }, min_support_{ min_support } {
		}

		virtual void calculate() = 0;

		void writeToFile() {
			auto fileName = getFileName();
			std::ofstream myfile(fileName);
			auto items = getItems();
			if (myfile.is_open()) {
				myfile << "Name of algoritm: " << name_ <<std::endl;
				myfile << items;
				myfile.close();
			}
			else {
				std::cerr << "Error: Unable to open the file for writing." << std::endl;
			}
		}

		std::vector<std::pair<gsp::item, size_t>> getItems() {
			return frequent_items_;
		};

		std::string getFileName() {
			std::string input = name_;
			for (size_t i = 0; i < input.length(); ++i) {
				if (input[i] == ' ') {
					input[i] = '_';
				}
			}
			return input + "_output" + ".txt";
		}

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

		void print(const map_items& items) {
			std::cout << "-------------------------" << std::endl;
			for (const auto& item : items) {
				for (const auto& ev : item.first) {
					std::cout << ev << " ";
				}
				std::cout << item.second << std::endl;
			}
			std::cout << "-------------------------" << std::endl;
		}

		void update(const map_items& items) {
			for (const auto& el : items) {
				frequent_items_.push_back({ el.first, el.second });
			}
		}
		const std::string name_;
		const std::vector<gsp::item>& data_base_;
		size_t min_support_;
		std::vector<std::pair<gsp::item, size_t>> frequent_items_;
	};
}