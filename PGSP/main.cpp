#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <queue>
#include <filesystem>
#include <memory>

#include "src/core/data_parser.h"
#include "src/cpu/GspEngineCpu.h"
#include "src/cpu/HashEngineCpu.h"
#include "src/cpu/SPSPMEngineCpu.h"
#include "src/gpu/SPSPMEngineGpu.h"
#include "src/gpu/HashEngineGpu.h"
#include "src/core/args_parser.h"

int main(int argc, char *argv[]) {
    using namespace std;

    auto args = gsp::parseArgs(argc, argv);
    auto debug_logs = gsp::checkDebugFlag(args);
    auto info_logs = gsp::checkInfoFlag(args);
    auto console_print = gsp::checkConsolePrint(args);
    auto [write_to_file, file_name] = gsp::checkWriteToFile(args);
    auto [gsp_status, gsp_type] = gsp::checkGspTypes(args);


    constexpr size_t num_of_work_group = 12;
    constexpr size_t min_support = 52;
    gsp::DataParser data_parser;
    // const vector<gsp::item> data_base = data_parser.getSimpleDataSet();
    std::filesystem::path inputPath = "./input/data_set0.txt";
    const vector<gsp::item> data_base = data_parser.readFromFile(inputPath);


    std::vector<shared_ptr<gsp::IEngine>> engines;

    engines.push_back(std::make_shared<gsp::GspEngineCpu>(data_base, min_support));

    engines.push_back(std::make_shared<gsp::HashEngineCpu>(data_base, min_support, num_of_work_group));

    engines.push_back(std::make_shared<gsp::SPSPMEngineCpu>(data_base, min_support, num_of_work_group));

    engines.push_back(std::make_shared<gsp::SPSPMEngineGpu>(data_base, min_support, num_of_work_group));

    engines.push_back(std::make_shared<gsp::HashEngineGpu>(data_base, min_support, num_of_work_group));

    for(auto& engine : engines){
        engine->calculate();
        auto items = engine->getItems();
        //gsp::print(hash_items);
        engine->writeToFile();
    }

    return 0;
}