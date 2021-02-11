register_block_pair({
    name = "Chest",
    id = "chests.normal",
    textures = {
        "Assets/chest_normal_front.png", "Assets/chest_normal_side.png", "Assets/chest_normal_side.png",
        "Assets/chest_normal_side.png", "Assets/chest_normal_top.png", "Assets/chest_normal_bottom.png"
    },
    image = "Assets/chest_normal_front.png",
    rotH = true,
})

onbreak = function(_pos)
    core.log_info("Called")
    voxel.map.getBlock(_pos)
end

register_block_pair({
    name = "Iron Chest",
    id = "chests.iron", --Jack Co. Supply Crate ;3
    textures = {
        "Assets/chest_iron_front.png", "Assets/chest_iron_side.png", "Assets/chest_iron_side.png",
        "Assets/chest_iron_side.png", "Assets/chest_iron_top.png", "Assets/chest_iron_bottom.png"
    },
    image = "Assets/chest_iron_front.png",
    rotH = true,
    onBreak = onbreak
})