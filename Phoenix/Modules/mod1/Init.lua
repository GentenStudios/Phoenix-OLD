core.log_info("Load mod 1")

function hello (args)
    if args[1] == "there" then
        print("General Kenobi")
    elseif args[1] == "world" then
        print("World says hi")
    else
        print("with you, the force is not")
    end
end

--core.command.register("Hello", "Master the arts of the Jedi you must", hello)

local Mod1TestSetting = core.settings.get("Mod1.Test1", 100)
print(Mod1TestSetting)

voxel.block.register({ name = "Dirt", id = "core.dirt", textures = { "Assets/dirt.png" } })

voxel.block.register({
    name = "Grass",
    id = "core.grass",
    textures = {
        "Assets/grass_side.png", "Assets/grass_side.png", "Assets/grass_side.png",
        "Assets/grass_side.png", "Assets/grass_top.png", "Assets/dirt.png"
    }
})

voxel.block.register({
    name = "Dirt Stair",
    id = "core.dirt_stair",
    model = "Stair",
    rotH = true,
    rotV = true,
    textures = { "Assets/dirt.png" }
})

voxel.block.register({
    name = "Test XPanel",
    id = "core.test_xpanel",
    model = "XPanel",
    rotH = true,
    textures = { "Assets/1.png", "Assets/2.png", "Assets/3.png", "Assets/4.png" }
})

voxel.block.register({
    name = "Test XPanelBlock",
    id = "core.test_xpanel_block",
    model = "XPanelCube",
    rotH = true,
    textures = {
        "Assets/1.png", "Assets/2.png", "Assets/3.png", "Assets/4.png",
        "Assets/dirt.png", "Assets/dirt.png", "Assets/dirt.png", "Assets/dirt.png",
        "Assets/dirt.png", "Assets/dirt.png", "Assets/dirt.png"
    }
})

voxel.block.register({
    name = "Test Slope",
    id = "core.test_slope",
    model = "Slope",
    rotH = true,
    rotV = true,
    textures = { "Assets/xpanel.png", "Assets/1.png", "Assets/2.png", "Assets/3.png", "Assets/dirt.png" }
})
