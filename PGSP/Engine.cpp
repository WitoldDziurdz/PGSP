#include "Engine.h"
#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <queue>

#include "utils.h"
#include <CL/sycl.hpp>



namespace gsp {

	size_t getCount(const std::vector<std::vector<std::string>>& data_base) {

	}

	std::set<char> Engine::generateUniqItems(const std::vector<std::vector<std::string>>& data_base) {
		std::set<char> uniq_items;
		for (const auto& items : data_base) {
			for (const auto& item : items) {
				uniq_items.insert(item.cbegin(), item.cend());
			}
		}
		return uniq_items;
	}


	void Engine::calculate(const std::vector<gsp::item>& data_base) {
		cl::sycl::queue queue(device_selector);
		std::cout << "Running on "
			<< queue.get_device().get_info<cl::sycl::info::device::name>()
			<< "\n";
		std::set<char> uniq_items = generateUniqItems(data_base);

		std::vector<std::vector<gsp::item>> client_data = gsp::split(data_base, num_of_work_group_);


		std::vector<std::vector<gsp::item>> pipes;
		pipes.resize(num_of_work_group_);

		{
			cl::sycl::buffer<std::vector<gsp::item>, 1> b_pipes(pipes.data(), pipes.size());
			cl::sycl::buffer<std::vector<gsp::item>, 1> b_data_base(client_data.data(), client_data.size());

			queue.submit([&](cl::sycl::handler& cgh) {
				auto a_b_pipes = b_pipes.get_access<cl::sycl::access::mode::read_write>(cgh);

				cgh.parallel_for_work_group<class generate_uniqitem>(cl::sycl::range<1>(num_of_work_group_), [=](cl::sycl::id<1> i) {
					size_t a = i;
				});
			});
		}
	}
}