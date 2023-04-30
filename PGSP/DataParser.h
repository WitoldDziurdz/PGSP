#pragma once
#include <string>
#include <vector>
#include <map>

#include "utils.h"

namespace gsp {

    class DataParser {

    public:
        std::vector<gsp::item> getSimpleDataSet() {
            std::vector<gsp::item> data_base = {
              { "t", "s", "g", "g", "l", "n", "s", "fst", "s", "l" },
              { "s", "ns", "s", "g", "k", "l", "l", "s", "k", "l", "g" },
              { "glnt", "s", "s", "a", "c", "p", "a", "l", "b" },
              { "s", "b", "g", "b", "glps", "c", "c" },
              { "gns", "s", "s", "ls", "n", "g", "g" },
              { "r", "s", "s", "s" },
              { "g", "g", "g", "p", "t", "p", "gpr" }
            };
            return data_base;
        }

        std::vector<std::vector<std::string>> readFromFile(const std::filesystem::path& filePath) {
            std::vector<std::vector<std::string>> result;
            std::ifstream inputFile(filePath);

            if (!inputFile.is_open()) {
                std::cerr << "Unable to open the file: " << filePath << std::endl;
                return result;
            }

            std::string line;
            while (std::getline(inputFile, line)) {
                std::vector<std::string> row;
                std::istringstream lineStream(line);
                std::string word;

                while (std::getline(lineStream, word, ',')) {
                    row.push_back(word);
                }

                result.push_back(row);
            }

            inputFile.close();
            return result;
        }
    };
}