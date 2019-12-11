#include <iostream>
#include <fstream>
#include <exception>
#include <string>
#include <map>
#include <chrono>

//#define TINYGLTF_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//
#include "tinygltf/tiny_gltf.h"

#include "rt.h"
#include "image.h"

int main(int argc, const char * argv[]) {



//    tinygltf::TinyGLTF loader;
//    tinygltf::Model model;
//
//    std::string err;
//    std::string warn;
//
//    bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, argv[1]);
////    bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, argv[1]);
////    printf("Cameras: %d\n", model.cameras.size());

    rt::Scene scene;

    rt::RenderOptions renderOptions;
    renderOptions.maxDepth = 20;
    renderOptions.threadCount = 8;
    renderOptions.horizontalResolution = 320;
    renderOptions.verticalResolution = 320;
    renderOptions.samplesPerPixel = 1000;

    rt::Image image(renderOptions.horizontalResolution, renderOptions.verticalResolution);

    rt::RenderContext renderContext;
    renderContext.scene = &scene;
    renderContext.options = &renderOptions;
    renderContext.image = &image;

    rt::rayTrace(renderContext);

    std::ofstream out("my.pgm");
    image.writeBinaryPgm(out);

    scene.getMatAtIdx(0);



    return 0;
}






