#include <catch2/catch_test_macros.hpp>

#include "vector-simple.hpp"
#include "test-helpers.hpp"

TEST_CASE("Reserving capacity does not construct any elements", "[vector][complex]") {
	vector<tracker> vec;
	const auto cnt = tracker::cnt;
	vec.reserve(10);
	REQUIRE(vec.capacity() == 10);
	REQUIRE(cnt == tracker::cnt);
}

TEST_CASE("Move constructor does not touch the elements", "[vector][complex]") {
	vector<tracker> vec1;
	vec1.push_back(tracker{ 1 });
	vec1.push_back(tracker{ 2 });
	vec1.push_back(tracker{ 4 });

	const auto cnt = tracker::cnt;
	auto vec2(std::move(vec1));
	REQUIRE(cnt == tracker::cnt);
	REQUIRE(vec2.size() == 3);
}

TEST_CASE("Move assignment destroys the old elements and adopts memory", "[vector][complex]") {
	vector<tracker> vec1, vec2;
	vec1.push_back(tracker{ 1 });
	vec1.push_back(tracker{ 2 });
	vec1.push_back(tracker{ 4 });
	vec1.push_back(tracker{ 8 });

	vec2.push_back(tracker{ 3 });
	vec2.push_back(tracker{ 9 });
	const auto cnt = tracker::cnt;
	vec2 = std::move(vec1);
	auto cnt2 = tracker::cnt;
	cnt2.destructors -= 2;
	REQUIRE(cnt2 == cnt);
}

TEST_CASE("push_back provides strong exception guarantee", "[vector][complex]") {
	vector<thrower> vec;
	vec.reserve(2);
	REQUIRE(vec.capacity() == 2);

	vec.push_back(thrower{ 1, false });
	vec.push_back(thrower{ 2, false });
	thrower t{ 4, true };
	REQUIRE_THROWS(vec.push_back(t));
	REQUIRE(vec.size() == 2);
	REQUIRE(vec.capacity() == 2);
	REQUIRE(vec[0].value == 1);
	REQUIRE(vec[1].value == 2);
}
