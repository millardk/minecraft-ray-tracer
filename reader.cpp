//
// Created by Keegan Millard on 2019-12-12.
//

#include "reader.h"

#include <fstream>

using namespace rt;

//Example 2
//Load PNG file from disk to memory first, then decode to raw pixels in memory.


Material matFromHex(int r, int g, int b) {
    Material ret;
    const Vector3d scaledValues = Vector3d(
            r/255.0L,
            g/255.0L,
            b/255.0L);

    ret.ka = 0.3 * scaledValues;
    ret.kd = 0.7 * scaledValues;
    ret.ks = 0.9 * scaledValues;
    ret.ns = 10;

    return ret;
}

Reader::Reader(std::string assetPath): assetPath(assetPath) {}

void Reader::readBlocks(std::string blockFilePath) {
    std::ifstream in(blockFilePath);
    while (in.peek() != EOF) {

        RawBlock block;
        in >> block.id;
        in >> block.x;
        in >> block.y;
        in >> block.z;

        uniqueBlocks.insert(block.id);
        blocks.push_back(block);
    }

}

/*
 *  2   grass
 *  9   water
 *  12  sand
 *  31  dead shrub
 *  38  poppy
 *  175 sunflower
 */

void Reader::readAssets() {












    auto defaultMat = new SolidColorMaterial(matFromHex(255,255,255));
    auto waterMat = new SolidColorMaterial(matFromHex(156, 211, 219));
    auto dirtMat = new SolidColorMaterial(matFromHex(155, 118, 83));
    auto grassMat = new SolidColorMaterial(matFromHex(86, 125, 70));
    auto sandMat = new SolidColorMaterial(matFromHex(194, 178, 128));

    BlockMaterial defaultBlock = BlockMaterial::makeUniformBlock(defaultMat);
    BlockMaterial grass = BlockMaterial::makeTopAndSideBlock(grassMat, dirtMat);
    BlockMaterial sand = BlockMaterial::makeUniformBlock(sandMat);
    BlockMaterial water = BlockMaterial::makeUniformBlock(waterMat);

    blockMaterials[0] = defaultBlock;
    blockMaterials[GRASS] = grass;
    blockMaterials[WATER] = water;
    blockMaterials[SAND] = sand;

}

void Reader::readIntoScene(Scene &s) {
    std::vector<Block> blockVector;
    for (RawBlock rb : blocks) {
        Block b;
        b.position = Vector3i(rb.x,rb.y,rb.z);
        b.type = rb.id;
        blockVector.push_back(b);
    }
    s.boxStore = new BoxStore(blockVector);
    s.blockMaterials = blockMaterials;
}
