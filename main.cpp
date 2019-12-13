#include <iostream>
#include <fstream>
#include <exception>
#include <string>
#include <map>
#include <chrono>

#include "rt.h"
#include "image.h"
#include "reader.h"

void setScene1(rt::Scene &s, double widthAspect, double heightAspect) {
    using namespace rt;

    auto blockFilePath = "./map/my-minecraft-map.txt";

    Reader r;
    r.readBlocks(blockFilePath);
    r.readAssets();
    r.readIntoScene(s);

//    LightSource light;
//    light.position = Vector3d( -100,80,100);
//    light.emittance = Vector3d(1,1,1);
//    s.lights.push_back(light);

    double lightAmt = 1.5;
    int lightCount = 4;
    double l = lightAmt/lightCount;

    LightSource light;
    light.position = Vector3d(-200,200,1050);
    light.emittance = Vector3d(l,l,l);
    s.lights.push_back(light);

    LightSource light2;
    light2.position = Vector3d(-198,200,1052);
    light2.emittance = Vector3d(l,l,l);
    s.lights.push_back(light2);

    LightSource light3;
    light3.position = Vector3d(-199,198,1051);
    light3.emittance = Vector3d(l,l,l);
    s.lights.push_back(light3);

    LightSource light4;
    light4.position = Vector3d(-200,203,1050);
    light4.emittance = Vector3d(l,l,l);
    s.lights.push_back(light4);

    s.ambientLight = Vector3d(0.3,0.3,0.3);

    auto *camera = new PerspectiveCamera;
    camera->focalPoint = Vector3d(-70,90,1080);
    camera->lookPoint = Vector3d(-45,70,1150);
    camera->upVector = Vector3d(0,1,0);
    camera->imagePlaneDistance = 2.0;

    camera->vB1 = -heightAspect;
    camera->vB2 = heightAspect;
    camera->hB1 = -widthAspect;
    camera->hB2 = widthAspect;
    camera->init();

    s.camera = camera;

    Sphere s1;
    s1.type = -1;
    s1.r = 5;
    s1.p = Vector3d(-74, 67, 1127);
    s.spheres.push_back(s1);

    Sphere s2;
    s2.type = -1;
    s2.r = 5;
    s2.p = Vector3d(-45, 85, 1172);
    s.spheres.push_back(s2);

    Sphere s3;
    s3.type = -1;
    s3.r = 5;
    s3.p = Vector3d(-33, 75, 1149);
    s.spheres.push_back(s3);

    //~ Sphere s4;
    //~ s4.type = -1;
    //~ s4.r = 0.5;
    //~ s4.p = Vector3d(-17.5, 66, 1133);
    //~ s.spheres.push_back(s4);

    Sphere s5;
    s5.type = -1;
    s5.r = 5;
    s5.p = Vector3d(21, 67, 1153);
    s.spheres.push_back(s5);



    Material sm;
    sm.ka = Vector3d(0.1,0.1,0.1);
    sm.kd = Vector3d(0.2,0.2,0.2);
    sm.ks = Vector3d(1.0,1.0,1.0);
    sm.kr = Vector3d(1.0,1.0,1.0);
    sm.ns = 10;
    s.sphereMaterials.push_back(sm);




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
    skySphere.radiiScale = 1.8;
    s.skySphere = skySphere;
}

void setScene2(rt::Scene &s, double widthAspect, double heightAspect) {
	using namespace rt;
	auto blockFilePath = "./map/my-minecraft-map.txt";

    Reader r;
    r.readBlocks(blockFilePath);
    r.readAssets();
    r.readIntoScene(s);
    
    LightSource light;
    light.position = Vector3d(-200,200,1050);
    light.emittance = Vector3d(1,1,1);
    s.lights.push_back(light);

    s.ambientLight = Vector3d(0.3,0.3,0.3);

    auto *camera = new PerspectiveCamera; 
    camera->focalPoint = Vector3d(0,120,1200);
    camera->lookPoint = camera->focalPoint - Vector3d(0,20,0);
    camera->upVector = Vector3d(0,1,0);
    camera->imagePlaneDistance = 0.5;
    
    camera->vB1 = -heightAspect;
    camera->vB2 = heightAspect;
    camera->hB1 = -widthAspect;
    camera->hB2 = widthAspect;
    camera->init();
    s.camera = camera;


    //~ Sphere s1;
    //~ s1.type = -1;
    //~ s1.r = 5;
    //~ s1.p = Vector3d(-74, 67, 1127);
    //~ s.spheres.push_back(s1);
   
    s.skyBoxEnabled = false;
}

int main(int argc, const char * argv[]) {
    using namespace rt;


    if (argc < 3) {
		std::cout << "usage: sceneNum heightRes threadCount(default 8)\n";
		return 1;
	}
	
	int sceneNum = std::stoi(argv[1]);
	int resScale = std::stoi(argv[2]);
	
	
	
    double heightAspect = 1;
    double widthAspect = 1.6;
    
    Scene scene;
    if (sceneNum == 1) {
		setScene1(scene, widthAspect, heightAspect);
	} else if (sceneNum == 2) {
	    setScene2(scene, widthAspect, heightAspect);
	}

    RenderOptions renderOptions;
    renderOptions.maxDepth = 10;
    renderOptions.threadCount = 8;
	if (argc == 4) {
		renderOptions.threadCount = std::stoi(argv[3]);
	}
    renderOptions.horizontalResolution = resScale * widthAspect;
    renderOptions.verticalResolution = resScale * heightAspect;

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

    std::ofstream out("./output/scene"+std::to_string(sceneNum)+"_"+myTime+".ppm");
    image.writeBinaryPgm(out);

    scene.getMaterial(0, 0, 0, 0);

    return 0;
}






