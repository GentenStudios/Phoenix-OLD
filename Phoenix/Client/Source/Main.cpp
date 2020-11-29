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

#include <Client/Client.hpp>

#include <Common/Utility/Serializer.hpp>

#include <fstream>

using namespace phx;

#undef main
int main(int argc, char** argv)
{
	// client::Client::get()->run();

	struct MetaData
	{
		std::string key;
		int         value;
		int         somethingElse;

		bool operator==(const MetaData& rhs) const
		{
			return rhs.key == key && rhs.value == value &&
			       rhs.somethingElse == somethingElse;
		}
	};

	std::string air = "core:air";
	MetaData    meta {"light", 5, 1232};

	struct Block
	{
		std::string name;
		MetaData    meta;
	};
	
	{
		std::vector<Block> blocks;

		for (std::size_t i = 0; i < 4096; ++i)
		{
			blocks.push_back({air, meta});
		}

		// setup data to push into ser.
		Serializer ser;

		// "run length encoded" serialize to file.
		{
			std::string lastBlockKey       = blocks[0].name;
			MetaData    lastBlockMeta      = blocks[0].meta;
			std::size_t currentRepeatCount = 0;
			for (auto& val : blocks)
			{
				if (val.name == lastBlockKey && val.meta == lastBlockMeta)
				{
					++currentRepeatCount;
				}
				else
				{
					ser << currentRepeatCount;
					ser << lastBlockKey;
					ser << lastBlockMeta.key;
					ser << lastBlockMeta.value;
					ser << lastBlockMeta.somethingElse;

					lastBlockKey  = val.name;
					lastBlockMeta = val.meta;
				}
			}

			ser << currentRepeatCount++;
			ser << lastBlockKey;
			ser << lastBlockMeta.key;
			ser << lastBlockMeta.value;
			ser << lastBlockMeta.somethingElse;
		}

		std::basic_ofstream<std::byte> output("save.save",
		                                      std::ios_base::binary);
		auto&                          buf = ser.getBuffer();
		output.write(buf.data(), buf.size());
		output.close();
	}

	std::basic_ifstream<std::byte> inFile("save.save", std::ios_base::binary);
	std::vector<Block>             blocks2;

	Serializer             ser;
	{
		std::vector<std::byte> loadBuf;

		inFile.seekg(0, std::ios_base::end);
		std::size_t length = inFile.tellg();
		inFile.seekg(0, std::ios_base::beg);

		loadBuf.resize(length);
		inFile.read(loadBuf.data(), length);

		// you can use appendToBuffer when reading files if there's already data in the serializer.
		// you should usually use setBuffer though.
		ser.setBuffer(std::move(loadBuf));
	}

	while (!ser.empty())
	{
		std::size_t repeats;
		std::string key;
		MetaData    metaData;

		ser >> repeats;
		ser >> key;
		ser >> metaData.key;
		ser >> metaData.value;
		ser >> metaData.somethingElse;

		for (std::size_t i = 0; i < repeats; ++i)
		{
			blocks2.push_back({key, metaData});
		}
	}
	
	for (auto& key : blocks2)
	{
		std::cout << key.name << " ";
	}
	std::cout << blocks2.size();

	return 0;
}
