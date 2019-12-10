#include <iostream>
#include <fstream>
#include <exception>
#include <string>
#include <map>


//#define TINYGLTF_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_IMPLEMENTATION
//
#include "gltf/tiny_gltf.h"


#include "Json.h"


int main(int argc, const char * argv[]) {

//    std::ifstream in(argv[1]);
//    std::string jsonString(std::istreambuf_iterator<char>(in), {});
//
////    json::printJsonTokens(jsonString);
//
//
//    json::Document document(jsonString);
//    auto name = document.getValue().getObject()["\"hobbies\""].getArray()[0].getString();
//    std::cout << name << "\n";


    tinygltf::TinyGLTF loader;
    tinygltf::Model model;

    std::string err;
    std::string warn;


    bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, argv[1]);
//    bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, argv[1]);

//    printf("Cameras: %d\n", model.cameras.size());

    model.buffers[0];
    model.materials.at(0)

    return 0;
}






//
//
//    if (!warn.empty()) {
//        printf("Warn: %s\n", warn.c_str());
//    }
//
//    if (!err.empty()) {
//        printf("Err: %s\n", err.c_str());
//    }
//
//    if (!ret) {
//        printf("Failed to parse glTF\n");
//        return -1;
//    }