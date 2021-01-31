#include <catch2/catch.hpp>

#include <Common/Voxels/Inventory.hpp>

using namespace phx::voxels;

ItemType* addTestItem(ItemReferrer& referrer)
{
	auto     uid = referrer.referrer.size();
	ItemType temp;
	temp.displayName      = &"Test"[uid];
	temp.id               = &"core.test"[uid];
	temp.uniqueIdentifier = uid;
	temp.maxStack         = 64;
	referrer.referrer.add(temp.id, uid);
	referrer.items.add(uid, temp);
	return referrer.items.get(uid);
}

TEST_CASE("Validate Inventory Behavior")
{
	ItemReferrer referrer;
	Item         itemA = {addTestItem(referrer), nullptr, 1};
	Item         itemB = {addTestItem(referrer), nullptr, 1};

	GIVEN("A newly created inventory")
	{
		Inventory inventory(20, &referrer);
		REQUIRE(inventory.getSize() == 20);
		REQUIRE(inventory.getItem(5).type == nullptr);

		WHEN("An item is inserted into the next open slot")
		{
			inventory.addItem(itemA);
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
		WHEN("A full stack of Items is inserted into an empty slot")
		{
			itemA.volume = 64;
			inventory.addItem(itemA);
			THEN("The full stack of items is moved into the slot")
			{
				REQUIRE(inventory.getItem(0).volume == 64);
				REQUIRE(itemA.volume == 0);
			}
		}
		GIVEN("A partial stack of items in an inventory")
		{
			itemA.volume = 32;
			inventory.addItem(0, itemA);
			WHEN("A full stack of the same item is inserted into a partially "
			     "filled slot")
			{
				itemA.volume = 64;
				REQUIRE(inventory.addItem(0, itemA) == false);
				THEN("Only part of the stack is inserted")
				{
					REQUIRE(itemA.volume == 32);
					REQUIRE(inventory.getItem(0).volume == 64);
				}
			}
			WHEN("A full stack of Items is inserted into the next open slot")
			{
				itemA.volume = 64;
				inventory.addItem(itemA);
				THEN("The items are split amongst two slots")
				{
					REQUIRE(inventory.getItem(0).volume == 64);
					REQUIRE(inventory.getItem(1).volume == 32);
				}
			}
			WHEN("A full stack of a different item is inserted into the next "
			     "open slot")
			{
				itemB.volume = 64;
				inventory.addItem(itemB);
				THEN("The entire stack is in the next slot and the existing "
				     "stack is not modified")
				{
					REQUIRE(inventory.getItem(0).volume == 32);
					REQUIRE(inventory.getItem(0).type == itemA.type);
					REQUIRE(inventory.getItem(1).volume == 64);
					REQUIRE(inventory.getItem(1).type == itemB.type);
				}
			}
		}
	}
}