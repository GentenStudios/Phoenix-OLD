#include <Common/Voxels/Block.hpp>
#include <glm/glm.hpp>
#include <Common/Voxels/Voronoi.h>
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
        static bool get(glm::vec2 pos, int id);
        static int get(glm::vec2 pos);


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

        static BlockType* getTerrainBlockAt(glm::vec3 pos);

};