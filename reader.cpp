//
// Created by Keegan Millard on 2019-12-12.
//

#include "reader.h"

#include <fstream>

using namespace rt;

//Example 2
//Load PNG file from disk to memory first, then decode to raw pixels in memory.




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

//    auto grass_top_tex = new Texture("./textures/green_wool.PPM");
    auto grass_side_tex = new Texture("./textures/grass_side.PPM");
    auto diamond_block_tex = new Texture("./textures/diamond_block.PPM");
    auto water_still_tex = new Texture("./textures/water_still.PPM");
    auto sand_tex = new Texture("./textures/sand.PPM");
    auto log_oak_tex = new Texture("./textures/log_oak.PPM");
    auto wool_colored_red_tex = new Texture("./textures/wool_colored_red.PPM");
    auto wool_colored_green_tex = new Texture("./textures/wool_colored_green.PPM");
    auto dirt_tex = new Texture("./textures/dirt.PPM");
    auto gravel_tex = new Texture("./textures/gravel.PPM");
    auto coal_ore_tex = new Texture("./textures/gravel.PPM");
    auto diamond_ore_tex = new Texture("./textures/diamond_ore.PPM");



    auto defaultMat = new SolidColorMaterial(Material::matFromHex(255,255,255));
//    auto waterMat = new SolidColorMaterial(matFromHex(156, 211, 219));
//    auto dirtMat = new SolidColorMaterial(matFromHex(155, 118, 83));
//    auto grassMat = new SolidColorMaterial(matFromHex(86, 125, 70));
//    auto sandMat = new SolidColorMaterial(matFromHex(194, 178, 128));

//    auto grassTopMat = new TextureMaterial(wool_colored_green_tex);
    auto grassSideMat = new TextureMaterial(grass_side_tex);
    auto waterMat = new TextureMaterial(water_still_tex);
    auto sandMat = new TextureMaterial(sand_tex);
    auto logOakMat = new TextureMaterial(log_oak_tex);
    auto diamondBlockMat = new TextureMaterial(diamond_block_tex);
    auto redWoolMat = new TextureMaterial(wool_colored_red_tex);
    auto greenWoolMat = new TextureMaterial(wool_colored_green_tex);
    auto dirtMat = new TextureMaterial(dirt_tex);
    auto gravelMat = new TextureMaterial(gravel_tex);




    BlockMaterial defaultBlock = BlockMaterial::makeUniformBlock(defaultMat);
    BlockMaterial grass = BlockMaterial::makeTopAndSideBlock(greenWoolMat, grassSideMat);
    BlockMaterial dirt = BlockMaterial::makeUniformBlock(dirtMat);
    BlockMaterial sand = BlockMaterial::makeUniformBlock(sandMat);
    BlockMaterial water = BlockMaterial::makeUniformBlock(waterMat);
    BlockMaterial log = BlockMaterial::makeUniformBlock(logOakMat);
    BlockMaterial wool = BlockMaterial::makeUniformBlock(redWoolMat);
    BlockMaterial leaves = BlockMaterial::makeUniformBlock(greenWoolMat);
    BlockMaterial gravel = BlockMaterial::makeUniformBlock(gravelMat);


    blockMaterials[0] = defaultBlock;
    blockMaterials[GRASS] = grass;
    blockMaterials[DIRT] = dirt;
    blockMaterials[WATER] = water;
    blockMaterials[SAND] = sand;
    blockMaterials[LOG] = log;
    blockMaterials[WOOL] = wool;
    blockMaterials[LEAVES] = leaves;

}

void Reader::readIntoScene(Scene &s) {
    std::vector<Block> blockVector;
    for (RawBlock rb : blocks) {
        Block b;
        b.position = Vector3i(rb.x,rb.y,rb.z);
        b.type = rb.id;
        blockVector.push_back(b);
    }
    s.blockStore = new BlockStore(blockVector);
    s.blockMaterials = blockMaterials;
}
