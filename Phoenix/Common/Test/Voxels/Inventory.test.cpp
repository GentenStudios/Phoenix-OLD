#include <catch2/catch.hpp>

#include <Common/Voxels/Inventory.hpp>

using namespace phx::voxels;

ItemType* addTestItem(ItemReferrer referrer)
{
	auto     uid = referrer.referrer.size();
	ItemType temp;
	temp.displayName      = &"Test"[uid];
	temp.id               = &"core.test"[uid];
	temp.uniqueIdentifier = uid;
	referrer.referrer.add(temp.id, uid);
	referrer.items.add(uid, temp);
	return referrer.items.get(uid);
}

TEST_CASE("Validate Inventory Behavior")
{
	ItemReferrer referrer;
	Item         itemA = {addTestItem(referrer), nullptr};
	Item         itemB = {addTestItem(referrer), nullptr};
	Item         itemC = {addTestItem(referrer), nullptr};

	GIVEN("A newly created inventory")
	{
		Inventory inventory(20, &referrer);
		REQUIRE(inventory.getSize() == 20);
		REQUIRE(inventory.getItem(5).type == nullptr);

		WHEN("An item is inserted into the next open slot")
		{
			REQUIRE(inventory.addItem(itemA) == 0);
			THEN("That item is in the next open slot")
			{
				REQUIRE(inventory.getItem(0).type == itemA.type);
			}
		}
		WHEN("An item is inserted into a specific slot")
		{
			REQUIRE(inventory.addItem(5, itemA) == true);
			THEN("That item is in the specified slot but not the next open one")
			{
				REQUIRE(inventory.getItem(5).type == itemA.type);
				REQUIRE(inventory.getItem(0).type == nullptr);
			}
		}
	}
}