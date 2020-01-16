Skip to content
Search or jump to…

Pull requests
Issues
Marketplace
Explore
 
@apachano 
GentenStudios
/
Phoenix
6
62
 Code Issues 20 Pull requests 6 Actions Projects 0 Security Insights Settings
Phoenix/Engine/Source/Player.cpp
@apachano apachano create player class that stores movement speed
36e1bdf 24 days ago
 52 lines (46 sloc)  1.89 KB
  
// Copyright 2019 Genten Studios
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

#include <Phoenix/Player.hpp>

const int MAX_MOVE_SPEED = 500;
const int DEFAULT_MOVE_SPEED = 10;

using namespace q2;

Player::Player() : m_moveSpeed(DEFAULT_MOVE_SPEED)
{};

int Player::getMoveSpeed()
{
    return m_moveSpeed;
}

bool Player::setMoveSpeed(int speed)
{
    if(MAX_MOVE_SPEED >= speed && speed >= 0)
    {
        m_moveSpeed = speed;
        return true;
    } 
    return false;
}