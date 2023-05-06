#pragma once
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <numeric>
#include <unordered_map>
#include <span>

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


    class DataBase {
    public:
        DataBase(std::string_view data_base, std::span<size_t> idx) : data_base_{ data_base }, idx_{ idx } {
            reset();
        }

        std::string_view operator[](size_t index) const {
            if (index + 1 == idx_.size()) {
                size_t nums = data_base_.size() - idx_[index];
                return std::string_view(data_base_.data() + idx_[index], nums);
            }
            size_t nums = idx_[index + 1] - idx_[index];
            return std::string_view(data_base_.data() + idx_[index], nums);
        }

        std::string_view getLine() {
            return this->operator[](current_index);
        }

        size_t nextLine() {
            if (current_index < idx_.size()) {
                ++current_index;
            }
            return current_index;
        }

        void reset() {
            current_index = 0;
        }

        size_t size() {
            return idx_.size();
        }

        class iterator {
        public:

            iterator(DataBase& db, size_t index) : db_{ &db }, index_{ index } {}

            iterator& operator++() {
                index_ = db_->nextLine();
                return *this;
            }

            std::string_view operator*() {
                return db_->getLine();
            }

            bool operator==(const iterator& other) const {
                return db_ == other.db_ && index_ == other.index_;
            }

            bool operator!=(const iterator& other) const {
                return !(*this == other);
            }

        private:
            DataBase* db_;
            size_t index_;
        };

        iterator begin() {
            reset();
            return iterator{ *this, 0 };
        }

        iterator end() {
            return iterator{ *this, idx_.size() };
        }
    private:
        size_t current_index;
        const std::string_view data_base_;
        const std::span<size_t> idx_;
    };
    


    class FlatElement {
    public:
        FlatElement(std::string_view s) : data(s) {}

        class iterator {
        public:
            iterator(std::string_view view) : remaining_(view) {
                update();
            }

            iterator& operator++() {
                update();
                return *this;
            }

            std::string_view operator*() const {
                return current_;
            }

            bool operator==(const iterator& other) const {
                return current_ == other.current_ && remaining_ == other.remaining_;
            }

            bool operator!=(const iterator& other) const {
                return !(*this == other);
            }

        private:
            size_t find(std::string_view str_view, char ch) {
                for (size_t i = 0; i < str_view.size(); ++i) {
                    if (str_view[i] == ch) {
                        return i;
                    }
                }
                return std::string_view::npos;
            }

            void update() {
                if (!remaining_.empty()) {
                    auto sep_pos = find(remaining_,',');
                    if (sep_pos != std::string_view::npos) {
                        current_ = std::string_view(remaining_.data(), sep_pos);
                        remaining_ = std::string_view(remaining_.data() + sep_pos + 1, remaining_.size() - sep_pos - 1);
                    }
                    else {
                        current_ = remaining_;
                        remaining_ = {};
                    }
                }
                else {
                    current_ = {};
                }
            }

            std::string_view remaining_;
            std::string_view current_;
        };

        iterator begin() const {
            return iterator(data);
        }

        iterator end() const {
            return iterator({});
        }

        size_t size() const {
            size_t count = 0;
            for (const auto& _ : *this) {
                ++count;
            }
            return count;
        }


    private:
        std::string_view data;
    };

}