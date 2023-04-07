#pragma once
#include <string>
#include <vector>
#include <set>
#include <numeric>
#include <thread>
#include <memory>

#include "queue.h"
#include "utils.h"

namespace gsp {

	class DataBase {

	public:
		DataBase(const std::vector<gsp::item>& data_base) : data_base_{ data_base }{
			reset();
		}

		std::vector<std::unique_ptr<gsp::item>> read(size_t numOfSize) {
			if (currentIndex_ < data_base_.size()) {
				const gsp::item& item = data_base_[currentIndex_++];
				return readItems(item, numOfSize);
			}
			else {
				return {};
			}
		}


		bool wasReaded() {
			return currentIndex_ == (data_base_.size() - 1);
		}

		void reset() {
			currentIndex_ = 0;
		}

	private:
		std::vector<std::unique_ptr<gsp::item>> readItems(const gsp::item& item, size_t numOfSize) {
			std::vector<std::unique_ptr<gsp::item>> elements;
			for (auto it = item.begin(); it != item.end(); it++) {
				std::unique_ptr<gsp::item> el = nullptr;

				if (std::distance(it, item.end()) > numOfSize) {
					el = std::make_unique<gsp::item>(std::vector<std::string>(it, it + numOfSize));
				}
				else {
					auto nums = std::accumulate(it, item.end(), 0, [](size_t a, const std::string& el) {
						return a + el.size();
					});
					if (nums < numOfSize) {
						break;
					}
					el = std::make_unique<gsp::item>(std::vector<std::string>(it, item.end()));
				}

				if (el) {
					elements.push_back(std::move(el));
				}
			}
			return elements;
		}


		size_t currentIndex_ = 0;
		const std::vector<gsp::item> data_base_;
	};
}