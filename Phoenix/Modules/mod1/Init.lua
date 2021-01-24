core.log_info("Load mod 1")

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

function register_block_pair(_name, _id, _textures, _image)
    local _obj = {
        name = _name,
        id = _id,
        places = _id,
        drops = _id,
        textures = _textures,
        image = _image
    }
    voxel.block.register(_obj)
    voxel.item.register(_obj)
end

register_block_pair("Dirt", "core.dirt", { "Assets/dirt.png" }, "Assets/dirt.png")
register_block_pair("Grass", "core.grass",
    {
        "Assets/grass_side.png", "Assets/grass_side.png", "Assets/grass_side.png",
        "Assets/grass_side.png", "Assets/grass_top.png", "Assets/dirt.png"
    }, "Assets/grass_side.png")

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

register_block_pair("Chest", "core.chest_normal",
    {
        "Assets/chest_normal_front.png", "Assets/chest_normal_side.png", "Assets/chest_normal_side.png",
        "Assets/chest_normal_side.png", "Assets/chest_normal_top.png", "Assets/chest_normal_bottom.png"
    }, "Assets/chest_normal_front.png")
register_block_pair("Iron Chest", "core.chest_iron", --Jack Co. Supply Crate ;3
    {
        "Assets/chest_iron_front.png", "Assets/chest_iron_side.png", "Assets/chest_iron_side.png",
        "Assets/chest_iron_side.png", "Assets/chest_iron_top.png", "Assets/chest_iron_bottom.png"
    }, "Assets/chest_iron_front.png")
	
