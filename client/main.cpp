//////////////
// #INCLUDE //
//////////////

// Library header:
#include "cg_engine.h"

#include "iostream"
#include "stdlib.h"
#include "string.h"
#include "tiffio.h"
#include "Geotiff.cpp"

// C/C++:
#include <iostream>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <chrono>

CgEngine* engine;
Node* scene;
Camera* staticCam;



glm::vec3 cameraPos = glm::vec3(100.0f, 600.0f, 100.0f);
glm::vec3 cameraFront = glm::vec3(500.0f, 400.0f, 500.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, -1.0f, 0.0f);
glm::vec3 direction;
float yaw, pitch = 0;

bool generateObjFile = false;

Node* search(Node* node, char* name) {
    if (strcmp(node->getName(), name) == 0) {
        return node;
    }

    for (int i = 0; i < node->getChildrenCount(); i++) {
        Node* res = search(node->getChild(i), name);

        if (res != nullptr) {
            return res;
        }
    }

    return nullptr;
}

/**
 * This callback is invoked each time a keyboard key is pressed.
 * @param key key pressed id
 * @param mouseX mouse X coordinate
 * @param mouseY mouse Y coordinate
 */
void keyboardCallback(unsigned char key, int mouseX, int mouseY)
{
    const float cameraSpeed = 50.0f; // adjust accordingly
    switch (key)
    {
        
        case 'w':
            staticCam->appendMatrix(glm::inverse(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 10.0f))));
            //cameraPos -= cameraSpeed * cameraFront;
            break;
        case 's':
            staticCam->appendMatrix(glm::inverse(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -10.0f))));
            //cameraPos += cameraSpeed * cameraFront;
            break;
        case 'a':
            staticCam->appendMatrix(glm::inverse(glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 0.0f, 0.0f))));
            //cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
            break;
        case 'd':
            staticCam->appendMatrix(glm::inverse(glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 0.0f, 0.0f))));
            //cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
            break;
        default:
            break;
    }
}

/**
 * This callback is invoked each time a special keyboard key is pressed.
 * @param key key pressed id
 * @param mouseX mouse X coordinate
 * @param mouseY mouse Y coordinate
 */
void specialCallback(int key, int mouseX, int mouseY)
{

    switch (key)
    {
        case 100://left
            staticCam->appendMatrix(glm::inverse(glm::rotate(glm::mat4(1.0f), glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f))));
            break;
        case 102://right
            staticCam->appendMatrix(glm::inverse(glm::rotate(glm::mat4(1.0f), glm::radians(-10.0f), glm::vec3(0.0f, 1.0f, 0.0f))));
            break;
        case 101://up
            staticCam->appendMatrix(glm::inverse(glm::rotate(glm::mat4(1.0f), glm::radians(10.0f), glm::vec3(1.0f, 0.0f, 0.0f))));
            break;
        case 103://down
            
            staticCam->appendMatrix(glm::inverse(glm::rotate(glm::mat4(1.0f), glm::radians(-10.0f), glm::vec3(1.0f, 0.0f, 0.0f))));
            break;
        default:
            break;
    }
}

Mesh* plane = new Mesh("plane");

float** readTiff(const char* path) {
    // create object of Geotiff class
    Geotiff* tiff = new Geotiff(path);

    cout << tiff->GetFileName() << endl;

    // dump out Geotiff band NoData value (often it is -9999.0)
    //cout << "No data value: " << tiff->GetNoDataValue() << endl;

    // dump out array (band) dimensions of Geotiff data  
    int* dims = tiff->GetDimensions();
    cout << dims[0] << " " << dims[1] << " " << dims[2] << endl;

    // output a value from 2D array  
    float** rasterBandData = tiff->GetRasterBand(1);

    delete tiff;

    return rasterBandData;
}

Chunk* generateChunk(float size, int tesselation, float** heights, int x, int z)
{
    if (size <= 0.0f || tesselation <= 0 || heights == nullptr) {
        std::cerr << "Invalid input parameters" << std::endl;
        return nullptr;
    }
    Chunk* chunk = new Chunk(x, z);

    std::cout << "Drawing Grid" << std::endl;
    std::cout << "Size: " << size << std::endl;
    std::cout << "Tesselation: " << tesselation << std::endl;
    // Compute starting coordinates and step size:
    float triangleSize = size / (float)tesselation;
    std::cout << "Triangle size: " << triangleSize << std::endl;

    // Generate all verticies
    cout << "Generating verticies..." << endl;
    chunk->verticies.reserve(size * size);
    for (int z = 0; z < size; z++) {
        for (int x = 0; x < size; x++) {
            chunk->addVertex(glm::vec3(x * triangleSize, heights[z][x], z * triangleSize));
        }
    }

    // Generate all triangles
    cout << "Generating faces..." << endl;
    chunk->faces.reserve((size - 1) * (size - 1) * 2);
    for (int y = 0; y < size - 1 ; y++) {
        for (int x = 0; x < size - 1; x++) {
            chunk->addFace(y * tesselation + x, (y + 1) * tesselation + x, y * tesselation + x +1);
            chunk->addFace((y+ 1) * tesselation + x, (y+1) * tesselation + x + 1, y * tesselation + x + 1);
        }
    }
    
    return chunk;
}

void generateObj(Mesh* mesh) {
    auto now = std::chrono::system_clock::now();
    auto UTC = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    std::ofstream objfile("./files/swissland_" + std::to_string(UTC) + ".obj");

    std::string text("");

    for (auto v : mesh->getVerticies()) {
        text.append("v ").append(std::to_string(v.x)).append(" ").append(std::to_string(v.y)).append(" ").append(std::to_string(v.z)).append("\n");
    }
    
    int i = 0;
    for (auto f : mesh->getFaces()) {
        f = f + 1;
        switch (i) {
        case 0:
            text.append("f ").append(std::to_string(f));
            i++;
            break;
        case 1:
            text.append(" ").append(std::to_string(f)).append(" ");
            i++;
            break;
        case 2:
            text.append(std::to_string(f)).append("\n");
            i = 0;
            break;
        }
    }

    objfile << text;

    objfile.close();
}

//////////
// MAIN //
//////////


using namespace std;
int main(int argc, char* argv[]) {
    // Init and use the lib:
    CgEngine* engine = CgEngine::getIstance();
    engine->init(argc, argv);

    // Set callbacks
    engine->setKeyboardCallback(keyboardCallback);
    engine->setSpecialCallback(specialCallback);

    // Load scene
    scene = new Node("[root]");
    scene->addChild(plane);

    float** rasterBandData = readTiff("../swissalti3d_2022_2709-1119_0.5_2056_5728.tif");
    Chunk* chunk = generateChunk(2000.0f, 2000.0f, rasterBandData, 0, 0);
    delete rasterBandData;
    plane->addChunk(chunk);
    delete chunk;

    rasterBandData = readTiff("../swissalti3d_2022_2709-1118_0.5_2056_5728.tif");
    chunk = generateChunk(2000.0f, 2000.0f, rasterBandData, 0, 1);
    delete rasterBandData;
    plane->addChunk(chunk);
    delete chunk;

    rasterBandData = readTiff("../swissalti3d_2022_2709-1120_0.5_2056_5728.tif");
    chunk = generateChunk(2000.0f, 2000.0f, rasterBandData, 0, -1);
    delete rasterBandData;
    plane->addChunk(chunk);
    delete chunk;

    rasterBandData = readTiff("../swissalti3d_2022_2710-1119_0.5_2056_5728.tif");
    chunk = generateChunk(2000.0f, 2000.0f, rasterBandData, 1, 0);
    delete rasterBandData;
    plane->addChunk(chunk);
    delete chunk;

    rasterBandData = readTiff("../swissalti3d_2022_2708-1119_0.5_2056_5728.tif");
    chunk = generateChunk(2000.0f, 2000.0f, rasterBandData, -1, 0);
    delete rasterBandData;
    plane->addChunk(chunk);
    delete chunk;

    plane->initVAO();

    if (generateObjFile) {
        generateObj(plane);
        cout << "Generated OBJ file" << endl;
    }

    // Add cameras to the scene
    staticCam = new PerspectiveCamera("camera", 1.0f, 3000.0f, 45.0f, 1.0f);
    glm::mat4 s_camera_M = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 600.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(240.0f), glm::vec3(0.0f, 1.0f, 0.0f));//* glm::rotate(glm::mat4(1.0f), glm::radians(-2.0f), glm::vec3(1.0f, 0.0f, 1.0f)) )
    //glm::mat4 s_camera_M = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    staticCam->setObjectCoordinates(s_camera_M);

    scene->addChild(staticCam);

    cout << "Added camera" << endl;

    // Parse selected scene and run
    engine->parse(scene);
    engine->run();

    engine->free();
    delete engine;

    return 0;
}