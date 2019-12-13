//
// Created by Keegan Millard on 2019-12-12.
//

#ifndef PATH_TRACER_READER_H
#define PATH_TRACER_READER_H

#include "rt.h"

#include <unordered_set>

namespace rt {

enum BlockType {
    STONE=1,
    GRASS=2,
    DIRT=3,
    WATER=9,
    SAND=12,
    GRAVEL=13,
    COAL_ORE=16,
    LOG=17,
    LEAVES=18,
    SANDSTONE=24,
    WOOL=35,
    DIAMOND_ORE=56,
    DIAMOND_BLOCK=57,
    GOLD_BLOCK=41,
    IRON_BLOCK=42,
};

class Reader {
private:

    struct RawBlock {
        int id;
        int x;
        int y;
        int z;
    };

    std::vector<RawBlock> blocks;

    std::unordered_map<int,BlockMaterial> blockMaterials;
    std::unordered_set<int> uniqueBlocks;

public:
    void readBlocks(std::string blockFilePath);
    void readAssets();
    void readIntoScene(Scene &s);

};

}

#endif //PATH_TRACER_READER_H
