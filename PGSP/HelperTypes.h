#pragma once
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <numeric>
#include <unordered_map>

namespace gsp {

    struct VectorOfStringHash {
        std::size_t operator()(const std::vector<std::string>& vec) const {
            std::size_t seed = 0;
            std::hash<std::string> hasher;
            for (const auto& str : vec) {
                seed ^= hasher(str) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }
            return seed;
        }
    };

    using item = std::vector<std::string>;
    using map_items = std::unordered_map<std::vector<std::string>, int, VectorOfStringHash>;

    class ItemIteartor {
    public:
        ItemIteartor(const item& items) : items_{ items } {
        }

        ItemIteartor& operator++() {
            ++str_index;
            adjust_indices();
            return *this;
        }

        ItemIteartor operator++(int) {
            ItemIteartor tmp(*this);
            operator++();
            return tmp;
        }

        bool operator==(const ItemIteartor& rhs) const {
            return items_ == rhs.items_ && vec_index == rhs.vec_index && str_index == rhs.str_index;
        }

        bool operator!=(const ItemIteartor& rhs) const {
            return !(*this == rhs);
        }

        char operator*() const {
            return items_[vec_index][str_index];
        }
    private:
        void adjust_indices() {
            while (vec_index < items_.size() && str_index >= items_[vec_index].size()) {
                ++vec_index;
                str_index = 0;
            }
        }
        size_t vec_index = 0;
        size_t str_index = 0;
        const item& items_;
    };
}