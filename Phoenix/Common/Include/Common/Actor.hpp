// Copyright 2019-20 Genten Studios
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors
// may be used to endorse or promote products derived from this software without
// specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

/**
 * @file Actor.hpp
 * @brief Header file for various components unique to Actors
 *
 * @copyright Copyright (c) 2019-2020 Genten Studios
 */

#include <Common/Input.hpp>
#include <Common/Voxels/Block.hpp>
#include <Common/Voxels/BlockReferrer.hpp>
#include <Common/Voxels/Item.hpp>
#include <Common/Voxels/Inventory.hpp>
#include <Common/Voxels/Map.hpp>

#include <entt/entt.hpp>

namespace phx
{
	/**
	 * @brief Wrapper for the player hotbar with bounds checked hand-slot
	 * variable accessors
	 */
	struct Hand
	{
	public:
		Hand(int hand, voxels::Inventory* inv)
		    : m_hand(hand), inventory(inv) {};
		voxels::Item getHand() const { return inventory->getItem(m_hand); };

		void setHandSlot(int slot)
		{
			if (slot < 0)
			{
				m_hand = size - 1;
			}
			else
			{
				m_hand = slot % size;
			}
		};
		int getHandSlot() const { return m_hand; };

		voxels::Inventory*   inventory;
		static constexpr int size = 10;

	private:
		int m_hand;
	};

	class ActorSystem
	{
	public:
		// TODO these should be unique to each actor
		static constexpr float m_reach    = 32.f;
		static constexpr bool  m_creative = false;

		static math::Ray    getTarget(entt::registry* registry,
		                              entt::entity    entity);

        static entt::entity registerActor(
		    entt::registry* registry,
		    voxels::Map*    map,
		    math::vec3      position,
		    math::vec3      rotation);
		static void         tick(entt::registry* registry, entt::entity entity,
		                         float dt, const InputState& input);
		static bool         action1(voxels::BlockReferrer* blockReferrer,
		                            entt::registry* registry, entt::entity entity);
		static bool         action2(voxels::BlockReferrer* blockReferrer,
		                            entt::registry* registry, entt::entity entity);
	};
} // namespace phx