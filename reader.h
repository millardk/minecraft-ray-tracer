//
// Created by Keegan Millard on 2019-12-12.
//

#ifndef PATH_TRACER_READER_H
#define PATH_TRACER_READER_H

#include "rt.h"

#include <unordered_set>

namespace rt {

enum BlockType {
    GRASS=2,
    WATER=9,
    SAND=12,
};


class Reader {
private:

    struct RawBlock {
        int id;
        int x;
        int y;
        int z;
    };

    std::string assetPath;
    std::vector<RawBlock> blocks;

    std::unordered_map<int,BlockMaterial> blockMaterials;
    std::unordered_set<int> uniqueBlocks;

public:
    Reader(std::string assetPath);

    void readBlocks(std::string blockFilePath);
    void readAssets();
    void readIntoScene(Scene &s);

};

}

#endif //PATH_TRACER_READER_H
