#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <queue>

#include "HashEngineCpu.h"
#include "utils.h"
#include <CL/sycl.hpp>




int main(int, char**) {
    using namespace std;
    constexpr size_t num_of_work_group = 5;

    const vector<gsp::item> data_base = {
            {"bd", "c", "b"},
            {"bf", "ce", "b"},
            {"ag", "b"},
            {"be", "ce"},
            {"a", "bd", "b", "c", "b"}
    };

    //const auto num_threads = std::thread::hardware_concurrency();
    gsp::HashEngineCpu engine(data_base, num_of_work_group);

    engine.calculate();

    return 0;
}