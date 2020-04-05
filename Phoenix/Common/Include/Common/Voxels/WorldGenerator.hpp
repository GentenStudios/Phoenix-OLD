#include <Common/Voxels/Block.hpp>
#include <Common/Voxels/Voronoi.h>
#include <Common/Math/Math.hpp>

#define JC_VORONOI_IMPLEMENTATION

using namespace phx::voxels;

class Voronoi {

    class Triangle {
        public:
            jcv_point a, b, c;
    };

    class Cell {
        public:
            int id;
            std::vector<Triangle> traingles;
    };

    public:
        static std::vector<Cell> cells;
        static void init();
        static bool get(phx::math::vec2 pos, int id);
        static int get(phx::math::vec2 pos);


};

class WorldGenerator {

    public:
        struct Params
        {
            float strength;
            float size;
            int octaves;
            float persistence;
            float height;
        };

        static Params params;

        static BlockType* getTerrainBlockAt(phx::math::vec3 pos);

};