#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <queue>
#include <filesystem>

#include "src/core/data_parser.h"
#include "src/cpu/GspEngineCpu.h"
#include "src/cpu/HashEngineCpu.h"
#include "src/cpu/SPSPMEngineCpu.h"
#include "src/gpu/SPSPMEngineGpu.h"
#include "src/gpu/HashEngineGpu.h"
#include "src/core/utils.h"
#include "src/core/helper_types.h"

#include <sycl/sycl.hpp>
#include <span>


int main(int, char**) {
    using namespace std;
    constexpr size_t num_of_work_group = 12;
    constexpr size_t min_support = 22;
    gsp::DataParser data_parser;
   // const vector<gsp::item> data_base = data_parser.getSimpleDataSet();
    std::filesystem::path inputPath = "./input/data_set0.txt";
    const vector<gsp::item> data_base = data_parser.readFromFile(inputPath);

    /*
    gsp::GspEngineCpu gsp_engine(data_base, min_support);
    gsp_engine.calculate();
    auto items = gsp_engine.getItems();
    gsp::print(items);
    gsp_engine.writeToFile();
    */
/*
    gsp::HashEngineCpu hash_engine(data_base, min_support, num_of_work_group);
    hash_engine.calculate();
    auto hash_items = hash_engine.getItems();
    //gsp::print(hash_items);
    hash_engine.writeToFile();

    gsp::SPSPMEngineCpu simple_engine(data_base, min_support, num_of_work_group);
    simple_engine.calculate();
    auto simple_items = simple_engine.getItems();
    //gsp::print(simple_items);
    simple_engine.writeToFile();
*/

    gsp::SPSPMEngineGpu gpu_simple_engine(data_base, min_support, num_of_work_group);
    gpu_simple_engine.calculate();
    auto gpu_simple_items = gpu_simple_engine.getItems();
    //gsp::print(gpu_simple_items);
    gpu_simple_engine.writeToFile();

    gsp::HashEngineGpu gpu_hash_engine(data_base, min_support, num_of_work_group*5);
    gpu_hash_engine.calculate();
    auto gpu_hash_items = gpu_hash_engine.getItems();
    //gsp::print(gpu_hash_items);
    gpu_hash_engine.writeToFile();


    return 0;
}