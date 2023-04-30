#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <queue>

#include "GspEngineCpu.h"
#include "HashEngineCpu.h"
#include "SPSPMEngineCpu.h"
#include "utils.h"
#include <CL/sycl.hpp>


int main(int, char**) {
    using namespace std;
    //constexpr size_t num_of_work_group = 5;

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
    gsp::GspEngineCpu gsp_engine(data_base, 2);
    gsp_engine.calculate();
    auto items = gsp_engine.getItems();
    gsp::print(items);

    gsp::HashEngineCpu hash_engine(data_base, 2, 5);
    hash_engine.calculate();
    auto hash_items = hash_engine.getItems();
    gsp::print(hash_items);

    gsp::HashEngineCpu simple_engine(data_base, 2, 5);
    simple_engine.calculate();
    auto simple_items = simple_engine.getItems();
    gsp::print(simple_items);
    return 0;
}