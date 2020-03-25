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

#include <iostream>

#include <Sandbox/Sandbox.hpp>

using namespace phx;

int main()
{
    entt::registry registry;
    std::uint64_t dt = 16;

    for (auto i = 0; i < 10; ++i)
    {
        auto entity = registry.create();
        registry.emplace<sandbox::position>(entity, i * 1.f, i * 1.f);
        if (i % 2 == 0)
            registry.emplace<sandbox::velocity>(entity, i * 0.1f, i * 0.1f);
    }

    std::cout << "BEFORE\n";
    registry.view<sandbox::position, sandbox::velocity>().each([dt](auto &pos, auto &vel) {
        std::cout << "entity(px: " << pos.x << ", py: " << pos.y << ", vx: " << vel.dx << ", vy: " << vel.dy << ")\n";
    });
    std::cout << "\n";

    sandbox::update(dt, registry);

    std::cout << "UPDATE 1\n";
    registry.view<sandbox::position, sandbox::velocity>().each([dt](auto &pos, auto &vel) {
        std::cout << "entity(px: " << pos.x << ", py: " << pos.y << ", vx: " << vel.dx << ", vy: " << vel.dy << ")\n";
    });
    std::cout << "\n";

    sandbox::update(registry);

    std::cout << "UPDATE 2\n";
    registry.view<sandbox::position, sandbox::velocity>().each([dt](auto &pos, auto &vel) {
        std::cout << "entity(px: " << pos.x << ", py: " << pos.y << ", vx: " << vel.dx << ", vy: " << vel.dy << ")\n";
    });
    std::cout << "\n";

    return 0;
}