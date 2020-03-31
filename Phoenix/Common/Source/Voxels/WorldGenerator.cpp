#include <Common/Voxels/WorldGenerator.hpp>

WorldGenerator::Params WorldGenerator::params;

void WorldGenerator::setParams(WorldGenerator::Params paramsIn)  {
    WorldGenerator::params = paramsIn;
}

WorldGenerator::Params WorldGenerator::getParams() {
    return WorldGenerator::params;
}