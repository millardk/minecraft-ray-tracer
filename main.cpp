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

void setScene1(rt::Scene &s) {
    using namespace rt;

    auto blockFilePath = "/Users/keegan/Desktop/Enklume-master/out.txt";
    auto assetPath = "/Users/keegan/Library/Application Support/minecraft/versions/1.15/1.15/assets/minecraft";

    Reader r(assetPath);
    r.readBlocks(blockFilePath);
    r.readAssets();
    r.readIntoScene(s);

//    LightSource light;
//    light.position = Vector3d( -100,80,100);
//    light.emittance = Vector3d(1,1,1);
//    s.lights.push_back(light);

    LightSource light;
    light.position = Vector3d(-100,90,1000);
    light.emittance = Vector3d(1,1,1);
    s.lights.push_back(light);

    s.ambientLight = Vector3d(0.3,0.3,0.3);


    auto *camera = new PerspectiveCamera;
    camera->focalPoint = Vector3d(-50,80,1090);
    camera->lookPoint = Vector3d(-25,64,1150);
    camera->upVector = Vector3d(0,1,0);
    camera->imagePlaneDistance = 1.5;

//    auto *camera = new PerspectiveCamera;
//    camera->focalPoint = Vector3d(-80,90,35);
//    camera->lookPoint = Vector3d(-75,70,55);
//    camera->upVector = Vector3d(0,1,0);
//    camera->imagePlaneDistance = 1;
//

    camera->vB1 = -1;
    camera->vB2 = 1;
    camera->hB1 = -1;
    camera->hB2 = 1;
    camera->init();

    s.camera = camera;

//    s.skyBoxEnabled = true;
//    s.textureSkySphereEnabled = false;
//    s.skyBoxColor = Vector3d(135, 206, 235) / 255.0L;

    s.skyBoxEnabled = true;
    s.textureSkySphereEnabled = true;
//    Texture *skyBoxTex = new Texture("./textures/sky_box2.PPM");
    Texture *skyBoxTex = new Texture("./textures/sky_box11.PPM");
    SkySphere skySphere;
    skySphere.material = TextureMaterial(skyBoxTex);
    skySphere.position = Vector3d(s.camera->lookPoint[0],-20, s.camera->lookPoint[2]);
    skySphere.radius = 200;
    s.skySphere = skySphere;



}


int main(int argc, const char * argv[]) {
    using namespace rt;

    Scene scene;
    setScene1(scene);

    RenderOptions renderOptions;
    renderOptions.maxDepth = 10;
    renderOptions.threadCount = 8;
    renderOptions.horizontalResolution = 500;
    renderOptions.verticalResolution = 500;

    rt::Image image(renderOptions.horizontalResolution, renderOptions.verticalResolution);

    rt::RenderContext renderContext;
    renderContext.scene = &scene;
    renderContext.options = &renderOptions;
    renderContext.image = &image;

    auto start = std::chrono::high_resolution_clock::now();

    rt::rayTrace(renderContext);

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count();
    std::cout << (double) duration / 1000 << "s\n";

    std::string myTime = std::to_string(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));

    std::ofstream out("./output/"+myTime+".ppm");
    image.writeBinaryPgm(out);

    scene.getMaterial(0, 0, 0, 0);

    return 0;
}






