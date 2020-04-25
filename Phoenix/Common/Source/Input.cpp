//
// Created by Nicolas Ricard on 19/04/2020.
//

#include <Common/Input.hpp>

phx::Serializer& phx::InputState::operator&(phx::Serializer& this_)
{
    return this_ & forward & backward & left & right & up & down & rotation.x & rotation.y & sequence;
}
