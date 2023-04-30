#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>
#include <thread>
#include <memory>
#include "utils.h"
#include "Node.h"
#include "IEngine.h"
#include <iostream>

namespace gsp {

	class HashEngineCpu : public IEngine{

	public:
		HashEngineCpu(const std::vector<gsp::item>& data_base, size_t min_support, size_t num_of_work_group) : IEngine(data_base, min_support), num_of_work_group_{num_of_work_group} {
			for (size_t i = 0; i < num_of_work_group_; ++i) {
				nodes_.push_back(std::make_unique<gsp::Node>(data_base, i, min_support, num_of_work_group));
			}
		}

		void calculate() override{
			//todo
			std::cout << "Nothing to calculate!" << std::endl;
		}

	private:
		const size_t num_of_work_group_;
		std::vector<std::unique_ptr<gsp::Node>> nodes_;
	};
}