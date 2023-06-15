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

    auto debug_logs = gsp::checkIfDebugFlag(args);
    auto info_logs = gsp::checkIfInfoFlag(args);
    auto console_print = gsp::checkIfConsolePrint(args);
    auto write_to_file = gsp::checkIfWriteToFile(args);
    auto gsps = gsp::checkGspTypes(args);
    auto [data_to_read, name_file_data] = gsp::checkIfReadDataFromFile(args);

    const size_t num_of_work_group = std::thread::hardware_concurrency();
    gsp::DataParser data_parser;
    vector<gsp::item> data_base = data_parser.getSimpleDataSet();
    if(data_to_read){
        std::filesystem::path inputPath = "./input/" + name_file_data;
        data_base = data_parser.readFromFile(inputPath);
    }

    long double float_min_sup = gsp::getMinSupport(args); //minimum support
    if (float_min_sup > 1 || float_min_sup <= 0) {
        std::cout << "Incorrect minimum support value\n";
        return 0;
    }
    const size_t min_support = ceil(data_base.size() * float_min_sup); ;

    std::cout << "Running gsp with float minimum support value: " << float_min_sup << " and min support: " << min_support << "\n";

    std::vector<shared_ptr<gsp::IEngine>> engines;
    if(gsps.contains(gsp::GspType::All)){
        engines.push_back(std::make_shared<gsp::GspEngineCpu>(data_base, min_support, info_logs, debug_logs));

        engines.push_back(std::make_shared<gsp::SPSPMEngineCpu>(data_base, min_support, num_of_work_group, info_logs, debug_logs));

        engines.push_back(std::make_shared<gsp::HashEngineCpu>(data_base, min_support, num_of_work_group, info_logs, debug_logs));

        engines.push_back(std::make_shared<gsp::SPSPMEngineGpu>(data_base, min_support, num_of_work_group, info_logs, debug_logs));

        engines.push_back(std::make_shared<gsp::HashEngineGpu>(data_base, min_support, num_of_work_group, info_logs, debug_logs));
    } else {
        if(gsps.contains(gsp::GspType::ClassicGsp)){
            engines.push_back(std::make_shared<gsp::GspEngineCpu>(data_base, min_support));
        }
        if(gsps.contains(gsp::GspType::SPSPMCpu)){
            engines.push_back(std::make_shared<gsp::SPSPMEngineCpu>(data_base, min_support, num_of_work_group, info_logs, debug_logs));
        }
        if(gsps.contains(gsp::GspType::HashGspCpu)){
            engines.push_back(std::make_shared<gsp::HashEngineCpu>(data_base, min_support, num_of_work_group, info_logs, debug_logs));
        }
        if(gsps.contains(gsp::GspType::SPSPMGpu)){
            engines.push_back(std::make_shared<gsp::SPSPMEngineGpu>(data_base, min_support, num_of_work_group, info_logs, debug_logs));
        }
        if(gsps.contains(gsp::GspType::HashGspGpu)){
            engines.push_back(std::make_shared<gsp::HashEngineGpu>(data_base, min_support, num_of_work_group, info_logs, debug_logs));
        }
    }

    for(auto& engine : engines){
        engine->calculate();
        if(console_print) {
            auto items = engine->getItems();
            gsp::print(items);
        }
        if(write_to_file) {
            engine->writeToFile();
        }
    }

    return 0;
}