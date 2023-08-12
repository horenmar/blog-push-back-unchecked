#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "vector-simple.hpp"

#include <vector>

namespace {
	struct Key {
		std::uint32_t id;
		friend size_t to_index(Key k) {
			return k.id;
		}
	};


	static std::vector<int> generate_data(size_t sz) {
		std::vector<int> data;
		data.reserve(sz);
		for (size_t i = 0; i < sz; ++i) {
			data.push_back(i);
		}
		return data;
	}

	static std::vector<Key> generate_key_data(size_t sz) {
		std::vector<Key> data;
		data.reserve(sz);
		for (size_t i = 0; i < sz; ++i) {
			data.push_back(Key{ static_cast<uint32_t>(i) });
		}
		return data;
	}
}

TEST_CASE("Copy ints", "[!benchmark]") {
	const auto num_elements = GENERATE(1'000, 100'000, 100'000'000);
	auto data = generate_data(num_elements);

	BENCHMARK("std::vector::push_back, n = " + std::to_string(num_elements)) {
		std::vector<int> out;
		out.reserve(data.size());
		for (int d : data) {
			out.push_back(2 * d);
		}
		return out;
	};

	BENCHMARK("my::vector::push_back, n = " + std::to_string(num_elements)) {
		vector<int> out;
		out.reserve(data.size());
		for (int d : data) {
			out.push_back(2 * d);
		}
		return out;
	};

	BENCHMARK("my::vector::push_back_unchecked, n = " + std::to_string(num_elements)) {
		vector<int> out;
		out.reserve(data.size());
		for (int d : data) {
			out.push_back_unchecked(2 * d);
		}
		return out;
	};
}

TEST_CASE("Transform to indices", "[!benchmark]") {
	const auto num_elements = GENERATE(1'000, 100'000, 100'000'000);
	auto data = generate_key_data(num_elements);

	BENCHMARK("std::vector::push_back, n = " + std::to_string(num_elements)) {
		std::vector<size_t> out;
		out.reserve(data.size());
		for (Key k : data) {
			out.push_back(to_index(k));
		}
		return out;
	};

	BENCHMARK("my::vector::push_back, n = " + std::to_string(num_elements)) {
		vector<size_t> out;
		out.reserve(data.size());
		for (Key k : data) {
			out.push_back(to_index(k));
		}
		return out;
	};

	BENCHMARK("my::vector::push_back_unchecked, n = " + std::to_string(num_elements)) {
		vector<size_t> out;
		out.reserve(data.size());
		for (Key k : data) {
			out.push_back_unchecked(to_index(k));
		}
		return out;
	};

}
