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

#include <Common/Voxels/Chunk.hpp>

#include <nlohmann/json.hpp>

#include <string>
#include <unordered_map>
#include <vector>

namespace phx
{
	// temporary class until map is updated.
	class Dimension
	{
	};

	/**
	 * @brief Configuration for a Save.
	 *
	 * All these values are read through a JSON file in the save directory. If
	 * the JSON file does not exist and/or the save itself doesn't exist, then a
	 * save will be needed to be made.
	 */
	struct SaveConfig
	{
		// name of save.
		std::string name;

		// mod list, passed as CLI arguments.
		std::vector<std::string> mods;

		// save specific settings, etc...
		nlohmann::json settings;
	};

	/**
	 * @brief A class to handle data within a save.
	 *
	 * Saves will be directories inside the Save/ folder. The saves will contain
	 * a JSON file containing settings for a specific save (not much currently,
	 * but it will definitely be useful down the line).
	 */
	class Save
	{
	public:
		/**
		 * @brief Loads a save, creates one if the parameters are provided.
		 * @param save The save to load.
		 * @param mods The mods to use if creating a save.
		 * @param settings The settings to use if creating a save.
		 */
		Save(const std::string& save, const std::vector<std::string>& mods = {}, const nlohmann::json& settings = {});

		/**
		 * @brief Saves everything to be saved and closes the save.
		 */
		~Save();

		/**
		 * @brief Gets the name of the save.
		 * @return The name of the save.
		 */
		const std::string& getName() const;

		/// @todo Implement Dimension system.
		/// keeping these empty methods so we know what we need, I'll work on
		/// this fairly soon with worldgen probably. - beeper.
		//Dimension* createDimension(const std::string& name);
		//Dimension* getDimension(const std::string& name);
		//void       setDefaultDimension(Dimension* dimension);
		//Dimension* getDefaultDimension();
		//const std::vector<std::string>& getDimensions() const;

		/**
		 * @brief Saves everything to be saved in the Save.
		 * @param name The name of the save.
		 *
		 * If the name is empty, it will overwrite/save to the existing save.
		 * Otherwise you will be able to "rename" the save, like a "save as".
		 *
		 * If renamed, the old save must be manually deleted.
		 */
		void toFile(const std::string& name = "");

	private:
		SaveConfig m_config;

		/**
		 * @brief Tells the saving operations whether settings have changed.
		 *
		 * If the settings have changed, then the JSON settings file will be
		 * updated, otherwise it won't be - it'll save a bit of time during
		 * saving, especially since there are a lot of chunks, etc... to spend
		 * time on instead.
		 */
		bool m_settingsChanged = false;

		//Dimension* m_defaultDimension;
		//std::unordered_map<std::string, Dimension> m_loadedDimensions;
	};
} // namespace phx
