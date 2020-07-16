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
		 * @brief Loads a save, quits if not found.
		 * @param save The save to load.
		 */
		Save(const std::string& save);

		/**
		 * @brief Saves everything to be saved and closes the save.
		 */
		~Save();

		/**
		 * @brief Creates a new save if one does not exist.
		 * @param config The configuration for the new save.
		 * @return The new save.
		 *
		 * If the save already exists it will return that save.
		 */
		static Save createSave(const SaveConfig& config);

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
		 */
		void save();

	private:
		SaveConfig m_data;

		//Dimension* m_defaultDimension;
		//std::unordered_map<std::string, Dimension> m_loadedDimensions;
	};
} // namespace phx
