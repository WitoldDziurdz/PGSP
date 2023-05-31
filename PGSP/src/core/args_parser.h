#pragma once
#include <string>
#include <vector>
#include <map>
#include <tuple>
#include <set>

#include "../IEngine.h"

namespace gsp {
    std::vector<std::string> parseArgs(int argc, char *argv[]){
        std::vector<std::string> args;
        for(int i = 0; i < argc; ++i){
            std::string str(argv[i]);
            args.push_back(str);
        }
        return args;
    }

    bool checkIfInfoFlag(std::vector<std::string>& args){
        auto it = std::find(args.begin(), args.end(), "--info");
        return it != args.end();
    }

    bool checkIfDebugFlag(std::vector<std::string>& args){
        auto it = std::find(args.begin(), args.end(), "--debug");
        return it != args.end();
    }

    bool checkIfConsolePrint(std::vector<std::string>& args){
        auto it = std::find(args.begin(), args.end(), "--print");
        return it != args.end();
    }

    bool checkIfWriteToFile(std::vector<std::string>& args){
        auto it = std::find(args.begin(), args.end(), "--file");
        if(it != args.end()){
            return true;
        }
        return false;
    }

    float getMinSupport(std::vector<std::string>& args){
        auto it = std::find(args.begin(), args.end(), "--min");
        if(it != args.end()){
            auto it_min_support = it + 1;
            if(it_min_support != args.end()){
                float num = std::stof(*it_min_support);
                return num;
            }
            return 0.01;
        }
        return 0.01;
    }

    std::tuple<bool, std::string> checkIfReadDataFromFile(std::vector<std::string>& args){
        auto it = std::find(args.begin(), args.end(), "--data");
        if(it != args.end()){
            auto it_file_name = it + 1;
            return std::make_tuple(true, *it_file_name);
        }
        return std::make_tuple(false, "");
    }

    std::set<gsp::GspType> checkGspTypes(std::vector<std::string>& args){
        std::set<gsp::GspType> results;

        auto it = std::find(args.begin(), args.end(), "--gsp");
        if(it != args.end()) {
            results.insert(gsp::GspType::ClassicGsp);
        }

        it = std::find(args.begin(), args.end(), "--sgsp_cpu");
        if(it != args.end()) {
            results.insert(gsp::GspType::SPSPMCpu);
        }

        it = std::find(args.begin(), args.end(), "--hgsp_cpu");
        if(it != args.end()) {
            results.insert(gsp::GspType::HashGspCpu);
        }

        it = std::find(args.begin(), args.end(), "--sgsp_gpu");
        if(it != args.end()) {
            results.insert(gsp::GspType::SPSPMGpu);
        }

        it = std::find(args.begin(), args.end(), "--hgsp_gpu");
        if(it != args.end()) {
            results.insert(gsp::GspType::HashGspGpu);
        }

        it = std::find(args.begin(), args.end(), "--all");
        if(it != args.end() || results.empty()) {
            results.insert(gsp::GspType::All);
        }
        return results;
    }

}
