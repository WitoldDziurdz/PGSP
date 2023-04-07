#pragma once
#include <vector>
#include <queue>
#include <mutex>
#include <optional>
#include <condition_variable>
#include <shared_mutex>
#include <thread>
#include <vector>
#include <map>

namespace gsp {

    template <typename T>
    class ConcurrentQueue {
    public:
        void push(std::unique_ptr<T> item) {
            std::unique_lock<std::mutex> lock(mutex_);
            queue_.push(std::move(item));
            lock.unlock();
            condition_variable_.notify_one();
        }

        std::unique_ptr<T> try_pop() {
            std::unique_lock<std::mutex> lock(mutex_);
            if (queue_.empty()) {
                return nullptr;
            }
            auto item = std::move(queue_.front());
            queue_.pop();
            return std::move(item);
        }

        std::unique_ptr<T> wait_and_pop() {
            std::unique_lock<std::mutex> lock(mutex_);
            condition_variable_.wait(lock, [this] { return !queue_.empty(); });
            auto item = std::move(queue_.front());
            queue_.pop();
            return std::move(item);
        }

        bool empty() const {
            std::lock_guard<std::mutex> lock(mutex_);
            return queue_.empty();
        }

    private:
        std::queue<std::unique_ptr<T>> queue_;
        mutable std::mutex mutex_;
        std::condition_variable condition_variable_;
    };


    template <typename T1, typename T2>
    class ConcurrentMap {
    public:
        T2& operator[](const T1& key) {
            std::unique_lock lock(mutex_);
            return map_[key];
        }

        T2& at(const T1& key) {
            std::shared_lock lock(mutex_);
            return map_.at(key);
        }

        void insert(const T1& key, const T2& value) {
            std::unique_lock lock(mutex_);
            map_[key] = value;
        }

        bool erase(const T1& key) {
            std::unique_lock lock(mutex_);
            return map_.erase(key) > 0;
        }

        size_t size() {
            std::shared_lock lock(mutex_);
            return map_.size();
        }

        size_t empty() {
            std::shared_lock lock(mutex_);
            return map_.size() == 0;

        }

        std::vector<std::pair<T1, T2>>  flat() {
            std::vector<std::pair<T1, T2>> elements;
            std::shared_lock lock(mutex_);
            for (const std::pair<T1, T2>& p : map_) {
                elements.push_back(p);
            }
            return elements;
        }

        std::vector<T1> keys() {
            std::vector<T1> elements;
            std::shared_lock lock(mutex_);
            for (const std::pair<T1, T2>& p : map_) {
                elements.push_back(p.first);
            }
            return elements;
        }


    private:
        std::map<T1, T2> map_;
        std::shared_mutex mutex_;
    };
}