#include <Common/Voxels/WorldGenerator.hpp>
#include <Common/Voxels/FastNoise.h>
#include <Common/Voxels/BlockRegistry.hpp>
#include <math.h>
#include <string.h>

#include <Common/Voxels/Voronoi.h>
#define JC_VORONOI_IMPLEMENTATION

using namespace phx;

WorldGenerator::Params WorldGenerator::params;

FastNoise heightNoise;
FastNoise riverNoise;
FastNoise heatNoise;

double getHeatMap(float posx, float posy, double height) {
    double noiseV = heatNoise.GetSimplex(posx/8, posy/8);

    double scalefactor = 4;

    return noiseV - pow(2,height)/scalefactor;
}

double getNormalizedHeightmap(float x, float y, float z, float strength, float size, int octaves, double persistence, float height) {
	double total = 0;
    double maxValue = 0;  // Used for normalizing result to 0.0 - 1.0
    for(int i=0;i<octaves;i++) {
        total += heightNoise.GetSimplex(x * size, y * size, z * size) * strength;
        
        maxValue += strength;
        
        strength *= persistence;
        size *= 2;
    }
    
    return (total/maxValue);
}

bool hasRiver(float x, float z) {
    double noiseV = riverNoise.GetSimplex(x/8,z/8);
    return noiseV > 0.1 && noiseV < 0.2;
}

double getHeightmap(float x, float y, float z, float strength, float size, int octaves, double persistence, float height) {
    return getNormalizedHeightmap(x, y, z, strength, size, octaves, persistence, height) * strength + height * strength;
}

double max = 0;

BlockType* WorldGenerator::getTerrainBlockAt(math::vec3 pos) {
    WorldGenerator::Params worldParams = WorldGenerator::params;

    double noiseV = getHeightmap(pos.x, pos.y, pos.z, worldParams.strength, worldParams.size, worldParams.octaves, worldParams.persistence, worldParams.height);

    double heat = getHeatMap(pos.x, pos.z, getNormalizedHeightmap(pos.x, pos.y, pos.z, worldParams.strength, worldParams.size, worldParams.octaves, worldParams.persistence, worldParams.height));

    BlockType* block = BlockRegistry::get()->getFromID("core:grass");

    /*if(pos.x > 0 && pos.z > 0 && false) {

        int id =  Voronoi::get(glm::vec2(pos.x, pos.z));
        glm::vec3 col = glm::vec3(((float) id)/256.);
        if(id > 0) {
            block->color = col;
            //std::cout << id << std::endl;
        }
    }*/

    block->color = math::vec3(1,1,1);
    
    if(pos.y < noiseV) {
        //if(Voronoi::get(glm::vec2(pos.x, pos.z), 68))
            return block;
        //else
        //    return BlockRegistry::get()->getFromID("core:air");

    }

    return BlockRegistry::get()->getFromID("core:air");

}

float randFloat() {
    return (static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
}

float triangleArea(jcv_point p1, jcv_point p2, jcv_point p3) {         //find area of triangle formed by p1, p2 and p3
   return abs((p1.x*(p2.y-p3.y) + p2.x*(p3.y-p1.y) + p3.x*(p1.y-p2.y))/2.0);
}

bool isInside(jcv_point p1, jcv_point p2, jcv_point p3, jcv_point p) {     //check whether p is inside or outside
   float area = triangleArea (p1, p2, p3);          //area of triangle ABC
   float area1 = triangleArea (p, p2, p3);         //area of PBC
   float area2 = triangleArea (p1, p, p3);         //area of APC
   float area3 = triangleArea (p1, p2, p);        //area of ABP

   return (area > (area1 + area2 + area3)-0.00001 && area < (area1 + area2 + area3)+0.00001);        //when three triangles are forming the whole triangle
}

std::vector<Voronoi::Cell> Voronoi::cells;

void printPoint(jcv_point p) {
    std::cout << "(" << p.x << ", " << p.y << ")" << std::endl;
}

void Voronoi::init() {

    std::vector<jcv_point> points;
    for(int i = 0; i < 16; i ++) {
        for(int j = 0; j < 16; j++) {
            jcv_point point;
            point.x = (i + randFloat()) * 16;
            point.y = (j + randFloat()) * 16;

            points.push_back(point);
        }
    }
    
    jcv_diagram diagram;
    memset(&diagram, 0, sizeof(jcv_diagram));
    jcv_diagram_generate(points.size(), points.data(), 0, 0, &diagram);

    const jcv_diagram* diagramP = &diagram;

    const jcv_site* sites = jcv_diagram_get_sites(diagramP);
    for(int i = 0; i < diagramP->numsites; ++i)
    {
        Voronoi::Cell cell;

        const jcv_site* site = &sites[i];

        const jcv_graphedge* e = site->edges;

        cell.id = i;

        while(e) {
            cell.traingles.push_back(Voronoi::Triangle{site->p, e->pos[0], e->pos[1]});
            e = e->next;
        }

        Voronoi::cells.push_back(cell);
    }
}

void print(std::string toPrint) {
    std::cout << toPrint << std::endl;
}


void print(int toPrint) {
    std::cout << toPrint << std::endl;
}

bool Voronoi::get(math::vec2 pos, int id) {

    for(int i = 0; i < Voronoi::cells.size(); ++i) {
        Voronoi::Cell cell = Voronoi::cells[i];

        if(cell.id == id)
            for(int j = 0; j < cell.traingles.size(); ++j) {
                Voronoi::Triangle tri = cell.traingles[j];

                if(isInside(tri.a, tri.b, tri.c, jcv_point{pos.x, pos.y})) {
                    return true;
                }
            }
    }
    return false;
}

int Voronoi::get(math::vec2 pos) {

    for(int i = 0; i < Voronoi::cells.size(); ++i) {
        Voronoi::Cell cell = Voronoi::cells[i];

        for(int j = 0; j < cell.traingles.size(); ++j) {
            Voronoi::Triangle tri = cell.traingles[j];

            if(isInside(tri.a, tri.b, tri.c, jcv_point{pos.x, pos.y})) {
                return cell.id;
            }
        }
    }
    return -1;
}
            /*
            printPoint(tri.a);
            printPoint(tri.b);
            printPoint(tri.c);
            print("---------");
            printPoint(jcv_point{pos.x, pos.y});
            print(cell.id);
            print("_________");
            */