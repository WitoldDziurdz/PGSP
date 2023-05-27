#pragma once
#include <string>
#include <vector>
#include <map>
#include <tuple>

#include "src/IEngine.h"

namespace gsp {
    std::vector<std::string> parseArgs(int argc, char *argv[]){
        std::vector<std::string> args;
        for(int i = 0; i < argc; ++i){
            std::string str(argv[i]);
            args.push_back(str);
        }
        return args;
    }

    bool checkInfoFlag(std::vector<std::string>& args){
        auto it = std::find(args.begin(), args.end(), "--info");
        return it != args.end();
    }

    bool checkDebugFlag(std::vector<std::string>& args){
        auto it = std::find(args.begin(), args.end(), "--debug");
        return it != args.end();
    }

    bool checkConsolePrint(std::vector<std::string>& args){
        auto it = std::find(args.begin(), args.end(), "--print");
        return it != args.end();
    }

    std::tuple<bool, std::string> checkWriteToFile(std::vector<std::string>& args){
        auto it = std::find(args.begin(), args.end(), "--file");
        if(it != args.end()){
            auto it_file_name = it + 1;
            return std::make_tuple(true, *it_file_name);
        }
        return std::make_tuple(true, "");
    }

    std::tuple<bool, gsp::GspType> checkGspTypes(std::vector<std::string>& args){
        auto it = std::find(args.begin(), args.end(), "--gsp");
        if(it != args.end()){
            auto it_type = it + 1;
            if(*it_type == "gsp"){
                return std::make_tuple(true, gsp::GspType::ClassicGsp);
            } else if(*it_type == "scpu"){
                return std::make_tuple(true, gsp::GspType::SPSPMCpu);
            } else if(*it_type == "sgpu"){
                return std::make_tuple(true, gsp::GspType::SPSPMGpu);
            } else if(*it_type == "hcpu"){
                return std::make_tuple(true, gsp::GspType::HashGspCpu);
            } else if(*it_type == "hgpu"){
                return std::make_tuple(true, gsp::GspType::HashGspGpu);
            }
            return std::make_tuple(true, gsp::GspType::All);
        }
        return std::make_tuple(true, gsp::GspType::All);
    }

}
