#pragma once
#include <string>
#include <vector>
#include <set>
#include <CL/sycl.hpp>

#include "utils.h"

namespace gsp {

	class Engine {

	public:
		Engine(size_t num_of_work_group) : num_of_work_group_ { num_of_work_group }{

		}

		std::set<char> generateUniqItems(const std::vector<std::vector<std::string>>& data_base);
		void calculate(const std::vector<gsp::item>& data_base);

	private:
		cl::sycl::default_selector device_selector;
		size_t num_of_work_group_;
	};
}