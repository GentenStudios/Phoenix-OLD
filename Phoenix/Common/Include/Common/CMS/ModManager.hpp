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

#pragma once

#include <Common/Logger.hpp>

#include <sol/sol.hpp>

#include <functional>
#include <queue>
#include <vector>

namespace phx::cms
{
	/**
	 * @brief A class to handle all Mods that require loading.
	 *
	 * To be used in conjunction with saves. Each save stores a list of mods
	 * associated to the world stored.
	 */
	class ModManager
	{
	public:
		/**
		 * @brief The status of the ModManager during load.
		 *
		 * This will help tell if there was an error while loading mods.
		 */
		struct Status
		{
			/// @brief Boolean saying whether the operation was ok or not.
			bool ok;

			/// @brief If the operation failed, the error message will be stored
			/// in this.
			std::string what;
		};

		using ModList = std::vector<std::string>;

	public:
		ModManager() = delete;

		/**
		 * @brief Constructs the manager by initializing the Lua state.
		 * @param toLoad The list of mods to load.
		 * @param paths The list of directories a mod could be found in.
		 *
		 * The allowance for extra paths to be used makes sure that someone can
		 * have mods stored in different directories. This is especially helpful
		 * for developers since they can work on their mod and store it
		 * somewhere much more convenient for them while still being able to use
		 * it.
		 *
		 * Also, this method registers all required base types (such as vec3 and
		 * the likes since there is no better place to put that stuff without
		 * coupling completely unrelated components).
		 */
		explicit ModManager(const ModList& toLoad, const ModList& paths);

		/**
		 * @brief Registers a C++ function into Lua.
		 * @tparam F The function signature for the function being registered.
		 * @param funcName The name the function should have in the Lua.
		 * @param func The function itself, can be a lambda or std::function.
		 *
		 * FuncName should be laid out like so:
		 *		namespace.namespace.functionName
		 *
		 * Function hierarchies of greater than 3 sections are not allowed, so
		 * only two namespaces are allowed as of right now. For example:
		 *		core.block.register(...)
		 */
		template <typename F>
		void registerFunction(const std::string& funcName, const F& func);

		/**
		 * @brief Registers a C++ class as a usertype in Lua.
		 * @tparam T The Class being exposed to Lua.
		 * @tparam Args Constructors that Lua will have access to
		 * @param name Name of the usertype in Lua, should follow PascalCase
		 * naming.
		 * @return The usertype object, used for registering member functions in
		 * lua.
		 *
		 * This function allows us to define user types in Lua see
		 * https://www.lua.org/pil/28.html for more information on user types.
		 *
		 * @note Member functions and variables are not accessible by default,
		 * you need to expose each member individually.
		 *
		 * @code{.cpp}
		 * auto typeVec3 = m_modManager->registerType<math::vec3>("Vec3",
		 * sol::constructors<math::vec3(), math::vec3(float, float, float)>());
		 * typeVec3["x"] = &math::vec3::x;
		 * typeVec3["y"] = &math::vec3::y;
		 * typeVec3["z"] = &math::vec3::z;
		 * @endcode
		 */
		template <typename T, typename... Args>
		sol::usertype<T> registerType(const std::string& name,
		                              sol::constructors<Args...>);

		// unimplemented function, exists as a reminder for when we need it.
		template <typename T>
		void exposeVariable(const std::string& name, T var);

		/**
		 * @brief Loads the mods into the Lua state, ready for the game.
		 * @param The current progress of the loading.
		 * @return The status of the load, can dictate fail or success.
		 */
		Status load(float* progress);

		/**
		 * @brief Cleans up mods once ready.
		 *
		 * Currently unimplemented, but will be implemented with a mod.cleanup
		 * function registered in lua for safely shutting down the game.
		 */
		void cleanup();

		/**
		 * @brief Gets the list of mods being loaded.
		 * @return The list of loaded mods.
		 */
		const ModList&     getModList() const;

		/**
		 * @brief Gets the path of the currently loading mod.
		 * @return The path of the mod being currently loaded.
		 *
		 * This is useful when loading mods since some functions don't have
		 * information on the currently loading mod, making it difficult to use
		 * things like textures that are packaged with the mod.
		 */
		const std::string& getCurrentModPath() const;

	private:
		std::vector<std::string> m_modsRequired;
		std::vector<std::string> m_modPaths;
		std::string              m_currentModPath;

		sol::state m_luaState;
	};
} // namespace phx::cms

#include "ModManager.inl"
