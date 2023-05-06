#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <queue>
#include <filesystem>

#include "DataParser.h"
#include "GspEngineCpu.h"
#include "HashEngineCpu.h"
#include "SPSPMEngineCpu.h"
#include "SPSPMEngineGpu.h"
#include "utils.h"
#include "HelperTypes.h"

#include <sycl/sycl.hpp>
#include <span>


int main(int, char**) {
    using namespace std;
    constexpr size_t num_of_work_group = 12;
    constexpr size_t min_support = 2;
    gsp::DataParser data_parser;
    auto data_base = data_parser.getSimpleDataSet();
    auto flat_data_base = data_parser.getFlatSimpleDataSet();
    std::cout << flat_data_base.first << std::endl;
   
    std::string_view data = flat_data_base.first;
    std::span<size_t> ids(flat_data_base.second.begin(), flat_data_base.second.end());
    gsp::DataBase dataBase(data, ids);
    for (auto line : dataBase) {
        std::cout << line << std::endl;
        for (auto item : gsp::FlatElement(line)) {
            std::cout << item << " ";
        }
        std::cout << std::endl;
    }
    gsp::SPSPMEngineGpu engine(data_base, min_support, num_of_work_group);



    engine.calculate();
    return 0;
}