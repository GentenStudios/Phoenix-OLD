print ("Load mod 1")
function hello (args)
    if args[1] == "there" then
        core.print("General Kenobi")
    elseif args[1] == "world" then
        core.print("World says hi")
    else
        core.print("with you, the force is not")
    end
end
core.command.register("Hello", "Master the arts of the Jedi you must", hello)
voxel.block.register("Dirt", "core:dirt",  
                    {"Assets/dirt.png"})
voxel.block.register("Grass", "core:grass",  
                    {"Assets/grass_side.png", "Assets/grass_side.png",
				    "Assets/grass_side.png", "Assets/grass_side.png",
				    "Assets/grass_top.png",  "Assets/dirt.png"})