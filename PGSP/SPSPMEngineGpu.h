#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>
#include <memory>
#include <iostream>
#include <algorithm>

#include <CL/sycl.hpp>

#include "utils.h"
#include "SimpleNode.h"
#include "IEngine.h"


namespace gsp {
    class SPSPMEngineGpu : public IEngine {

    public:
        SPSPMEngineGpu(const std::vector<gsp::item>& data_base, size_t min_support, size_t num_of_work_group): 
            IEngine("Simply Partitioned Sequential Pattern Mining GPU", data_base, min_support),
            num_of_work_group_{ num_of_work_group } { 

        }

        void calculate() override {
            cl::sycl::default_selector device_selector;
            cl::sycl::queue queue(device_selector);
            std::cout << "Running on " << queue.get_device().get_info<cl::sycl::info::device::name>() << "\n";


        }
    private:
        const size_t num_of_work_group_;
    };
}