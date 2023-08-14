#include <catch2/catch_test_macros.hpp>

#include "vector-simple.hpp"

TEST_CASE("Uninit alloc", "[helpers]") {
	auto* ptr = nonstd::Detail::allocate_uninit<int>(2);
	nonstd::Detail::deallocate_no_destroy(ptr, 2);
}

TEST_CASE("Simple vector", "[vector]") {
	nonstd::vector<int> vec;
	vec.reserve(2);
	REQUIRE(vec.capacity() == 2);

	vec.push_back_unchecked(1);
	vec.push_back_unchecked(2);
	REQUIRE(vec[0] == 1);
	REQUIRE(vec[1] == 2);
	vec.reserve(4);
	vec.push_back_unchecked(4);
	vec.push_back_unchecked(8);
	REQUIRE(vec[2] == 4);
	REQUIRE(vec[3] == 8);
	vec.reserve(2);
	REQUIRE(vec.size() == 4);
	REQUIRE(vec.capacity() == 4);
	REQUIRE(vec[2] == 4);
	REQUIRE(vec[3] == 8);
}

TEST_CASE("vector::push_back", "[vector]") {
	std::vector<int> target{ 1, 2, 3, 4 };
	nonstd::vector<int> vec;
	vec.push_back(1);
	vec.push_back(2);
	vec.push_back(3);
	vec.push_back(4);
	REQUIRE(std::equal(vec.begin(), vec.end(), target.begin(), target.end()));
}

TEST_CASE("vector::push_back element from vector", "[vector]") {
	nonstd::vector<int> vec;
	vec.push_back(1);
	for (size_t i = 0; i < 9; ++i) {
		vec.push_back(vec[0]);
	}

	REQUIRE(vec.size() == 10);
	for (size_t i = 0; i < 10; ++i) {
		REQUIRE(vec[i] == 1);
	}
}

TEST_CASE("vector moves", "[vector]") {
	nonstd::vector<int> vec1;
	vec1.push_back(2); vec1.push_back(0);
	SECTION("move constructor") {
		nonstd::vector<int> vec2(std::move(vec1));
		REQUIRE(vec2.size() == 2);
		REQUIRE(vec2[0] == 2);
		REQUIRE(vec2[1] == 0);
	}
	SECTION("move assignment") {
		nonstd::vector<int> vec2;
		vec2.push_back(3);
		vec2.push_back(3);
		vec2 = std::move(vec1);
		REQUIRE(vec2.size() == 2);
		REQUIRE(vec2[0] == 2);
		REQUIRE(vec2[1] == 0);
	}
}
