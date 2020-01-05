#include <Quartz2/Voxels/BlockRegistry.hpp>

#include <iostream>

int main(int argc, char* argv[])
{
	using namespace q2::voxels;

	BlockRegistry::instance()->initialise();

	BlockType grassBlock;
	{
		grassBlock.displayName = "Grass";
		grassBlock.id          = "core:grass";
		grassBlock.category    = BlockCategory::SOLID;

		// top, left, back, right, top, bottom
		grassBlock.textures = {
		    "grass.png", "dirt.png", "dirt.png",
		    "dirt.png",  "dirt.png", "dirt.png",
		};
	}

	BlockType dirtBlock;
	{
		dirtBlock.displayName = "Dirt";
		dirtBlock.id          = "core:dirt";
		dirtBlock.category    = BlockCategory::SOLID;

		//// top, left, back, right, top, bottom
		// dirtBlock.textures = {"dirt.png", "dirt.png", "dirt.png",
		//                       "dirt.png", "dirt.png", "dirt.png"};
		// OH WAIT, WE DON'T NEED TO DO THIS BIT LIKE THIS, WE CAN DO IT LIKE
		// THIS:
		dirtBlock.setAllTextures("dirt.png");
	}

	BlockRegistry::instance()->registerBlock(grassBlock);
	BlockRegistry::instance()->registerBlock(dirtBlock);

	TextureRegistry* registeredTextures =
	    BlockRegistry::instance()->getTextures();

	auto textures = registeredTextures->getTextures();

	for (auto texture : textures)
	{
		std::cout << "Texture: " << texture << std::endl;
	}
}
