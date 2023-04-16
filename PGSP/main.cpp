#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <queue>

#include "GspEngineCpu.h"
#include "utils.h"
#include <CL/sycl.hpp>




int main(int, char**) {
    using namespace std;
    constexpr size_t num_of_work_group = 5;

    const vector<gsp::item> data_base = {
        { "t", "s", "g", "g", "l", "n", "s", "fst", "s", "l" },
        { "s", "ns", "s", "g", "k", "l", "l", "s", "k", "l", "g" },
        { "glnt", "s", "s", "a", "c", "p", "a", "l", "b" },
        { "s", "b", "g", "b", "glps", "c", "c" },
        { "gns", "s", "s", "ls", "n", "g", "g" },
        { "r", "s", "s", "s" },
        { "g", "g", "g", "p", "t", "p", "gpr" }
    };

    //const auto num_threads = std::thread::hardware_concurrency();
    gsp::GspEngineCpu engine(data_base, 2, num_of_work_group);
    engine.calculate();
    auto items = engine.getItems();

    gsp::print(items);
    return 0;
}