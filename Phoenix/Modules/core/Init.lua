
--function hello (args)
--    if args[1] == "there" then
--        print("General Kenobi")
--    elseif args[1] == "world" then
--        print("World says hi")
--    else
--        print("with you, the force is not")
--    end
--end

--core.command.register("Hello", "Master the arts of the Jedi you must", hello)

core.setting.register({ name = "Test1", key = "Mod1.test1", default = 50, max = 100, min = 0 })
core.setting.register({ name = "Test2", key = "Mod1.test2", default = 50, max = 100, min = 0 })
core.setting.register({ name = "Test3", key = "Mod1.test3", default = 50, max = 100, min = 0 })

function register_block_pair(_obj)
    _obj.places = _obj.places or _obj.id
    _obj.drops = _obj.drops or _obj.id
    voxel.block.register(_obj)
    voxel.item.register(_obj)
end

register_block_pair({
    name = "Dirt",
    id = "core.dirt",
    textures = { "Assets/dirt.png" },
    image = "Assets/dirt.png"
})

register_block_pair({
    name = "Grass",
    id = "core.grass",
    textures = {
        "Assets/grass_side.png", "Assets/grass_side.png", "Assets/grass_side.png",
        "Assets/grass_side.png", "Assets/grass_top.png", "Assets/dirt.png"
    },
    image = "Assets/grass_side.png"
})

register_block_pair({
    name = "Dirt Stair",
    id = "core.dirt_stair",
    model = "Stair",
    rotH = true,
    rotV = true,
    textures = { "Assets/dirt.png" }
})

register_block_pair({
    name = "Test XPanel",
    id = "core.test_xpanel",
    model = "XPanel",
    rotH = true,
    textures = { "Assets/1.png", "Assets/2.png", "Assets/3.png", "Assets/4.png" }
})

register_block_pair({
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

register_block_pair({
    name = "Test Slope",
    id = "core.test_slope",
    model = "Slope",
    rotH = true,
    rotV = true,
    textures = { "Assets/xpanel.png", "Assets/1.png", "Assets/2.png", "Assets/3.png", "Assets/dirt.png" }
})
	
