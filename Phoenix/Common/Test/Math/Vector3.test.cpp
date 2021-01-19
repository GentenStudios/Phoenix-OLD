#include <catch2/catch.hpp>

#include <Common/Math/Vector3.hpp>

using namespace phx::math::detail;

TEST_CASE("Vec3 Addition", "[Vec3]")
{
	REQUIRE(Vector3<int> {1, 1, 1} + Vector3<int> {1, 1, 1} ==
	        Vector3<int> {2, 2, 2});
}