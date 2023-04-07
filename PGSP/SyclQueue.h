#pragma once
#include <string_view>
#include <unordered_map>
#include <array>

namespace gsp {
	template <size_t N = 256>
	class String {
	public:

		std::string_view getValue() {
			return std::string_view(&buffer_[0], size_)
		}

		void setValue(char* str, size_t l) {
			auto str_view = std::string_view(str, l);
			setValue(str_view);
		}

		void setValue(std::string_view str_view) {
			for (size_t i = 0; i < str_view.size(); ++i) {
				buffer_[i] = str_view[i];
			}
		}

		size_t getHash() {
			return std::hash<std::string_view>{}(getValue());
		}

	private:
		size_t size_ = 0;
		const size_t capacity_ = N;
		char buffer_[N];
	};

	template <size_t N>
	struct CanditateEntry {
		String<N> item;
		size_t nums = 0;

		std::string_view getValue() {
			return item.getValue();
		}
	};


	template<typename T, size_t N>
	class Queue {

		void push(const T& t) {
			queue_[size_++] = T;
		}

		T pop() {
			return queue_[--size_];
		}

		T front() {
			return queue_[size_-1]
		}

		bool contain(T t) {
			for (size_t i = 0; i < size_; ++i) {
				if (queue_[i].getValue() == t.getValue()) {
					return true;
				}
			}
			return false;
		}

	protected:
		std::array<T, N> queue_;
		size_t size_ = 0;
	};
}