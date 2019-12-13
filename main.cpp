#include <iostream>
#include <fstream>
#include <exception>
#include <string>
#include <map>
#include <chrono>

#include "rt.h"
#include "image.h"
#include "reader.h"

void test() {
    using namespace rt;

    Ray ray(Vector3d(0.2,0.5,-3), Vector3d(0,0,1));
    Block block(Vector3i(0,0,0), 0);
    HitRecord hit;

    bool res = block.doesHit(ray, hit);

    std::cout << res << '\n';
    std::cout << hit.distance << '\n';
    std::cout << hit.point << '\n';
    std::cout << hit.normal << '\n';

}

void setCameraAndLights(rt::Scene &s) {
    using namespace rt;

//    s.boxStore = new BoxStore({
//        Block(Vector3i(0,0,0),0),
//        Block(Vector3i(1,0,0),0),
//        Block(Vector3i(0,0,1),0),
//        Block(Vector3i(1,0,1),0),
//        Block(Vector3i(1,1,1),0),
//    });

//    LightSource light;
//    light.position = Vector3d( -100,80,100);
//    light.emittance = Vector3d(1,1,1);
//    s.lights.push_back(light);

    LightSource light;
    light.position = Vector3d(-75,80,55);
    light.emittance = Vector3d(1,1,1);
    s.lights.push_back(light);


//    LightSource light2;
//    light.position = Vector3d(5,5,0);
//    light.emittance = Vector3d(.5,.5,.7);
//    s.lights.push_back(light2);


    s.ambientLight = Vector3d(0.3,0.3,0.3);

    auto *camera = new PerspectiveCamera;
    camera->focalPoint = Vector3d(-90,100,35);
    camera->lookPoint = Vector3d(-75,70,55);
    camera->upVector = Vector3d(0,1,0);
    camera->imagePlaneDistance = 2;
    camera->vB1 = -1;
    camera->vB2 = 1;
    camera->hB1 = -1;
    camera->hB2 = 1;
    camera->init();

    s.camera = camera;
}




int main(int argc, const char * argv[]) {

//    test();

    using namespace rt;

    Texture t("./textures/grass_side.PPM");

    Pixel p = t.sampleAt(0,.5);
    std::cout << std::to_string(p.r) << " " << std::to_string(p.g) << " " << std::to_string(p.b) << '\n';

    Pixel p2 = t.sampleAt(1,.5);
    std::cout << std::to_string(p2.r) << " " << std::to_string(p2.g) << " " << std::to_string(p2.b) << '\n';

//    auto blockFilePath = "/Users/keegan/Desktop/Enklume-master/out.txt";
//    auto assetPath = "/Users/keegan/Library/Application Support/minecraft/versions/1.15/1.15/assets/minecraft";
//
//    Reader r(assetPath);
//    r.readBlocks(blockFilePath);
//    r.readAssets();
//
//    Scene scene;
//    r.readIntoScene(scene);
//    setCameraAndLights(scene);
//
//    RenderOptions renderOptions;
//    renderOptions.maxDepth = 1;
//    renderOptions.threadCount = 8;
//    renderOptions.horizontalResolution = 320;
//    renderOptions.verticalResolution = 320;
//
//    rt::Image image(renderOptions.horizontalResolution, renderOptions.verticalResolution);
//
//    rt::RenderContext renderContext;
//    renderContext.scene = &scene;
//    renderContext.options = &renderOptions;
//    renderContext.image = &image;
//
//    rt::rayTrace(renderContext);
//
//    std::ofstream out("my.pgm");
//    image.writeBinaryPgm(out);
//
//    scene.getMaterial(0, 0, 0, 0);

    return 0;
}






