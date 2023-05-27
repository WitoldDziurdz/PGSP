#pragma once
#include <string>
#include <vector>
#include <map>

#include "utils.h"
#include "profile.h"
namespace gsp {
    std::vector<gsp::item> convert(gsp::map_items frequent_items) {
        std::vector<gsp::item> result;
        result.reserve(frequent_items.size());

        std::transform(
                frequent_items.begin(), frequent_items.end(),
                std::back_inserter(result),
                [](gsp::map_items::value_type& item) {
                    return std::move(item.first);
                }
        );
        return result;
    }

    inline std::string toString(const gsp::item& item) {
        auto len = gsp::getSize(item) + item.size()*2;
        std::string str;
        str.reserve(len);
        bool isFirst = true;
        for (const auto& s : item) {
            if (!isFirst) {
                str += ',';
            }
            str += s;
            isFirst = false;
        }
        return str;
    }


    inline std::pair<std::string, std::vector<size_t>> convert(const std::vector<gsp::item>& data_base) {
        size_t len = std::accumulate(data_base.begin(), data_base.end(), 0, [](size_t s, const auto& item) {
            return s + gsp::getSize(item);
        });
        std::string str;
        str.reserve(len);
        std::vector<size_t> ids;
        ids.reserve(data_base.size());
        size_t sum = 0;
        for (size_t i = 0; i < data_base.size(); ++i) {
            ids.push_back(sum);
            auto line = toString(data_base[i]);
            sum += line.size();
            str += std::move(line);
        }
        ids.shrink_to_fit();
        str.shrink_to_fit();
        return { str, ids };
    }

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