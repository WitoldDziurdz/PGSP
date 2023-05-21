#pragma once
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <numeric>
#include <unordered_map>
#include <span>
#include <memory>

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

        size_t size() const {
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
                return delete_spaces(current_);
            }

            bool operator==(const iterator& other) const {
                return current_ == other.current_ && remaining_ == other.remaining_;
            }

            bool operator!=(const iterator& other) const {
                return !(*this == other);
            }

        private:

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

        std::string_view getLastElement() {
            size_t pos =rfind(data, ',');
            if (pos != std::string_view::npos) {
                auto str = std::string_view(data.data() + pos + 1, data.size() - pos - 1);
                return delete_spaces(str);
            } else {
                return delete_spaces(data);
            }
        }

        size_t size() const {
            size_t count = 0;
            for (const auto& _ : *this) {
                ++count;
            }
            return count;
        }



    private:
        static size_t find(std::string_view str_view, char ch) {
            for (size_t i = 0; i < str_view.size(); ++i) {
                if (str_view[i] == ch) {
                    return i;
                }
            }
            return std::string_view::npos;
        }

        static size_t rfind(std::string_view str_view, char ch) {
            for (size_t i = str_view.size(); i > 0; --i) {
                if (str_view[i - 1] == ch) {
                    return i - 1;
                }
            }
            return std::string_view::npos;
        }

        static std::string_view delete_spaces(std::string_view str){
            auto sep_pos = find(str,' ');
            if(sep_pos == std::string_view::npos){
                return str;
            }
            return std::string_view(str.data(), sep_pos);
        }

        std::string_view data;
    };

    class FlatArrayInterface {
    public:
        FlatArrayInterface(size_t rows, size_t cols, size_t string_size)
        : rows_(rows), cols_(cols), string_size_(string_size){
        }

        size_t getOffset(size_t row, size_t col) const {
            return  row * cols_ * string_size_ + col * string_size_;
        }

        size_t rows() const {
            return rows_;
        }

        size_t cols() const {
            return cols_;
        }

        size_t size() const {
            return rows_ * cols_ * string_size_;
        }

        size_t string_size() const{
            return string_size_;
        }

    protected:
        const size_t rows_;
        const size_t cols_;
        const size_t string_size_;
    };

    class FlatArray : public FlatArrayInterface{
        public:
            FlatArray(size_t rows, size_t cols, size_t string_size)
                    : FlatArrayInterface(rows, cols,string_size) {
                data_.resize(rows * cols * string_size);
                row_sizes.resize(rows);
                std::fill(row_sizes.begin(), row_sizes.end(), 0);
                std::fill(data_.begin(), data_.end(), ' ');
            }

            char* data() {
                return data_.data();
            }

            size_t* rowSizes() {
                return row_sizes.data();
            }


        private:
            std::vector<char> data_;
            std::vector<size_t> row_sizes;
        };


    class FlatArrayWrapper : public FlatArrayInterface{
    public:
        FlatArrayWrapper(char* data, size_t* row_sizes, size_t rows, size_t cols, size_t string_size)
                : FlatArrayInterface(rows, cols,string_size),
                  data_(data), row_sizes_(row_sizes) {
        }

        bool insert(size_t row, std::string_view str) {
            if(!find(row, str)){
                size_t current_col = row_sizes_[row];
                set(row, current_col, str);
                row_sizes_[row]++;
                return true;
            }
            return false;
        }

        char* getEntry(size_t row){
            size_t current_col = row_sizes_[row];
            row_sizes_[row]++;
            char* target = &data_[getOffset(row, current_col)];
            return target;
        }

        bool find(size_t row, std::string_view str){
            size_t current_size = row_sizes_[row];
            for(size_t col = 0; col < current_size; ++col){
                std::string_view source = get(row, col);
                if(source == str){
                    return true;
                }
            }
            return false;
        }

        std::string_view get(size_t row, size_t col) {
            const char* source = &data_[getOffset(row, col)];
            size_t len = string_size_;
            for(size_t i = 0; i < len; ++i){
                if(source[i] == ' '){
                    len = i;
                    break;
                }
            }
            return std::string_view(source, len);
        }

        void set(size_t row, size_t col, std::string_view str) {
            char* target = &data_[getOffset(row, col)];
            auto len = std::min(string_size_, str.size());
            size_t i = 0;
            for(i = 0; i < len; ++i) {
                target[i] = str[i];
            }
            for(; i < string_size_; ++i) {
                target[i] = ' ';
            }
        }

        size_t size(size_t row) {
            return row_sizes_[row];
        }

        char* data() {
            return data_;
        }

    private:
        size_t* row_sizes_;
        char* data_;
    };
}