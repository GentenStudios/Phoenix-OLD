print ("Load mod 1")
function hello (args)
    if args[1] == "there" then
        print("General Kenobi")
    elseif args[1] == "world" then
        print("World says hi")
    else
        print("with you, the force is not")
    end
end
core.command.register("Hello", "Master the arts of the Jedi you must", hello)

block = {}
block.name = "Dirt"
block.id = "core:dirt"
block.textures = {"Assets/dirt.png"}
voxel.block.register(block)

block = {}
block.name = "Grass"
block.id = "core:grass"
block.textures = {"Assets/grass_side.png", "Assets/grass_side.png",
"Assets/grass_side.png", "Assets/grass_side.png",
"Assets/grass_top.png",  "Assets/dirt.png"}
block.onBreak = function (position)
	print("grass broken at" + position)
end
voxel.block.register(block)

block = {}
block.name = "Air"
block.id = "core:air"
block.category = "Air"
voxel.block.register(block)

-- strength, size, octaves, persistence, height
world_params.setWorldParams(32, 0.4, 4, 1, 0.1)