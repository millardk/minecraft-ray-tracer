//
// Created by Keegan Millard on 2019-12-12.
//

#include "reader.h"

#include <fstream>

using namespace rt;

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

    const std::string tex_path = "./textures/";
//    auto grass_top_tex = new Texture(tex_path+"green_wool.PPM");
    auto grass_side_tex = new Texture(tex_path+"grass_side.PPM");
    auto diamond_block_tex = new Texture(tex_path+"diamond_block.PPM");
    auto water_still_tex = new Texture(tex_path+"water_still.PPM");
    auto sand_tex = new Texture(tex_path+"sand.PPM");
    auto log_oak_tex = new Texture(tex_path+"log_oak.PPM");
    auto wool_colored_red_tex = new Texture(tex_path+"wool_colored_red.PPM");
    auto wool_colored_green_tex = new Texture(tex_path+"wool_colored_green.PPM");
    auto wool_colored_silver_tex = new Texture(tex_path+"wool_colored_silver.PPM");
    auto dirt_tex = new Texture(tex_path+"dirt.PPM");
    auto gravel_tex = new Texture(tex_path+"gravel.PPM");
    auto coal_ore_tex = new Texture(tex_path+"coal_ore.PPM");
    auto diamond_ore_tex = new Texture(tex_path+"diamond_ore.PPM");
    auto stone_tex = new Texture(tex_path+"stone.PPM");
    auto sandstone_top_tex = new Texture(tex_path+"sandstone_top.PPM");
    auto sandstone_normal_tex = new Texture(tex_path+"sandstone_normal.PPM");
    auto gold_block_tex = new Texture(tex_path+"gold_block.PPM");

    auto pumpkin_side_tex = new Texture(tex_path+"pumpkin_side.PPM");
    auto pumpkin_top_tex = new Texture(tex_path+"pumpkin_side.PPM");
    auto melon_side_tex = new Texture(tex_path+"melon_side.PPM");
    auto melon_top_tex = new Texture(tex_path+"melon_side.PPM");
    auto cactus_side_tex = new Texture(tex_path+"cactus_side.PPM");
    auto cactus_top_tex = new Texture(tex_path+"cactus_side.PPM");


    MaterialPart watery(0.2,0.3,0.3,0.6,10);
    MaterialPart shiny(0.3,0.6,0.8,0.3,10);
    MaterialPart rough(0.3,0.8,0.6,0.2,10);

    auto defaultMat = new SolidColorMaterial(Pixel(255,255,255), rough);
//    auto waterMat = new SolidColorMaterial(matFromHex(156, 211, 219));
//    auto dirtMat = new SolidColorMaterial(matFromHex(155, 118, 83));
//    auto grassMat = new SolidColorMaterial(matFromHex(86, 125, 70));
//    auto sandMat = new SolidColorMaterial(matFromHex(194, 178, 128));

//    auto grassTopMat = new TextureMaterial(wool_colored_green_tex);
    auto grassSideMat = new TextureMaterial(grass_side_tex, rough);
    auto waterMat = new TextureMaterial(water_still_tex, watery);
    auto sandMat = new TextureMaterial(sand_tex, rough);
    auto logOakMat = new TextureMaterial(log_oak_tex, rough);
    auto diamondBlockMat = new TextureMaterial(diamond_block_tex, shiny);
    auto redWoolMat = new TextureMaterial(wool_colored_red_tex, rough);
    auto greenWoolMat = new TextureMaterial(wool_colored_green_tex, rough);
    auto silverWoolMat = new TextureMaterial(wool_colored_silver_tex, shiny);
    auto dirtMat = new TextureMaterial(dirt_tex, rough);
    auto gravelMat = new TextureMaterial(gravel_tex, rough);
    auto stoneMat = new TextureMaterial(stone_tex, shiny);
    auto sandStoneTopMat = new TextureMaterial(sandstone_top_tex, rough);
    auto sandStoneNormalMat = new TextureMaterial(sandstone_normal_tex, rough);
    auto diamondOreMat = new TextureMaterial(diamond_ore_tex, rough);
    auto goldBlockMat = new TextureMaterial(gold_block_tex, shiny);
    auto cactusTopMat = new TextureMaterial(cactus_top_tex, rough);
    auto cactusSideMat = new TextureMaterial(cactus_side_tex, rough);
    auto melonTopMat = new TextureMaterial(melon_top_tex, rough);
    auto melonSideMat = new TextureMaterial(melon_side_tex, rough);
    auto pumpkinTopMat = new TextureMaterial(pumpkin_top_tex, rough);
    auto pumpkinSideMat = new TextureMaterial(pumpkin_side_tex, rough);


    BlockMaterial defaultBlock = BlockMaterial::makeUniformBlock(defaultMat);
    BlockMaterial grass = BlockMaterial::makeTopAndSideBlock(greenWoolMat, grassSideMat);
    BlockMaterial dirt = BlockMaterial::makeUniformBlock(dirtMat);
    BlockMaterial sand = BlockMaterial::makeUniformBlock(sandMat);
    BlockMaterial water = BlockMaterial::makeUniformBlock(waterMat);
    BlockMaterial log = BlockMaterial::makeUniformBlock(logOakMat);
    BlockMaterial wool = BlockMaterial::makeUniformBlock(redWoolMat);
    BlockMaterial leaves = BlockMaterial::makeUniformBlock(greenWoolMat);
    BlockMaterial gravel = BlockMaterial::makeUniformBlock(gravelMat);
    BlockMaterial diamondBlock = BlockMaterial::makeUniformBlock(diamondBlockMat);
    BlockMaterial stone = BlockMaterial::makeUniformBlock(stoneMat);
    BlockMaterial sandStone = BlockMaterial::makeTopAndSideBlock(sandStoneTopMat, sandStoneNormalMat);
    BlockMaterial diamondOre = BlockMaterial::makeUniformBlock(diamondOreMat);
    BlockMaterial goldBlock = BlockMaterial::makeUniformBlock(goldBlockMat);
    BlockMaterial silverWool = BlockMaterial::makeUniformBlock(goldBlockMat);
    BlockMaterial pumpkin = BlockMaterial::makeTopAndSideBlock(pumpkinTopMat, pumpkinSideMat);
    BlockMaterial melon = BlockMaterial::makeTopAndSideBlock(melonTopMat, melonSideMat);
    BlockMaterial cactus = BlockMaterial::makeTopAndSideBlock(cactusTopMat, cactusSideMat);


    blockMaterials[0] = defaultBlock;
    blockMaterials[STONE] = stone;
    blockMaterials[GRASS] = grass;
    blockMaterials[DIRT] = dirt;
    blockMaterials[WATER] = water;
    blockMaterials[SANDSTONE] = sandStone;
    blockMaterials[SAND] = sand;
    blockMaterials[LOG] = log;
    blockMaterials[WOOL] = diamondBlock;
    blockMaterials[LEAVES] = leaves;
    blockMaterials[GRAVEL] = gravel;
    blockMaterials[DIAMOND_BLOCK] = diamondBlock;
    blockMaterials[DIAMOND_ORE] = diamondOre;
    blockMaterials[GOLD_BLOCK] = silverWool;
    blockMaterials[IRON_BLOCK] = goldBlock;
    blockMaterials[CACTUS] = cactus;
    blockMaterials[PUMPKIN] = pumpkin;
    blockMaterials[MELON] = melon;
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
