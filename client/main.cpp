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

bool generateObjFile = true;
std::string vectors = "";
std::string faces = "";


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
Chunk* chunk = new Chunk("chunk");

Mesh* drawGrid(float size, int tesselation, float** heights, float min)
{
    if (size <= 0.0f || tesselation <= 0 || heights == nullptr) {
        std::cerr << "Invalid input parameters" << std::endl;
        return nullptr;
    }

    std::cout << "Drawing Grid" << std::endl;
    std::cout << "Size: " << size << std::endl;
    std::cout << "Tesselation: " << tesselation << std::endl;
    // Compute starting coordinates and step size:
    float start = -size / 2.0f;
    float end = size / 2.0f;
    float triangleSize = size / (float)tesselation;
    std::cout << "Triangle size: " << triangleSize << std::endl;

    // Generate all verticies
    float curZ = start;
    float curX = start;

    int x = 0;
    int y = 0;

    cout << "Generating verticies..." << endl;

    while (curZ < end) {
        chunk->addVertex(glm::vec3(curX, heights[x][y], curZ));
        
        curX += triangleSize;
        x++;

        if (curX >= end) {
            curZ += triangleSize;
            curX = start;

            y++;
            x = 0;
        }
    }

    cout << "Generating faces..." << endl;

    // Generate all triangles
    for (int y = 0; y < tesselation -1 ; y++) {
        for (int x = 0; x < tesselation - 1; x++) {
            chunk->addFace(y * tesselation + x, (y + 1) * tesselation + x, y * tesselation + x +1);
            chunk->addFace((y+ 1) * tesselation + x, (y+1) * tesselation + x + 1, y * tesselation + x + 1);
        }
    }

    cout << "Loading verticies..." << endl;
    plane->addVerticies(chunk->getVertecies());

    cout << "Loading faces..." << endl;
    plane->addFaces(chunk->getFaces());

    delete chunk;

    cout << "Init VAO..." << endl;
    plane->initVAO();
    return plane;
}

void generateObj(Mesh* mesh) {
    auto now = std::chrono::system_clock::now();
    auto UTC = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    std::ofstream objfile("./files/swissland_" + std::to_string(UTC) + ".obj");

    for (auto v : mesh->getVertecies()) {
        objfile << "v " + std::to_string(v.x) + " " + std::to_string(v.y) + " " + std::to_string(v.z) + "\n";
    }
    
    int i = 0;
    for (auto f : mesh->getFaces()) {
        f = f + 1;
        switch (i) {
        case 0:
            objfile << "f " + std::to_string(f);
            i++;
            break;
        case 1:
            objfile << " " + std::to_string(f) + " ";
            i++;
            break;
        case 2:
            objfile << std::to_string(f) + "\n";
            i = 0;
            break;
        }
    }

    objfile.close();
}

//////////
// MAIN //
//////////


using namespace std;
int main(int argc, char* argv[]) {
    // create object of Geotiff class
    Geotiff* tiff = new Geotiff((const char*)"../swissalti3d_2022_2709-1119_0.5_2056_5728.tif");

    cout << tiff->GetFileName() << endl;

    // dump out Geotiff band NoData value (often it is -9999.0)
    //cout << "No data value: " << tiff->GetNoDataValue() << endl;

    // dump out array (band) dimensions of Geotiff data  
    int* dims = tiff->GetDimensions();
    cout << dims[0] << " " << dims[1] << " " << dims[2] << endl;

    // output a value from 2D array  
    float** rasterBandData = tiff->GetRasterBand(1);

    float min = 1000;
    float max = 0;
    for (int y = 0; y < dims[1]; y++) {
        for (int x = 0; x < dims[0]; x++) {
            if (rasterBandData[x][y] < min)
                min = rasterBandData[x][y];

            if (rasterBandData[x][y] > max)
                max = rasterBandData[x][y];
            //cout << "value at row " << x << ", column " << y << ": " << rasterBandData[x][y] << endl;
        }
    }

    cout << min << " " << max << endl;
    delete tiff;
    

    // Init and use the lib:
    CgEngine* engine = CgEngine::getIstance();
    engine->init(argc, argv);

    // Set callbacks
    engine->setKeyboardCallback(keyboardCallback);
    engine->setSpecialCallback(specialCallback);

    // Load scene
    scene = new Node("[root]");

    //TODO: problem with some numbers crashing drawGrid (tesselation examples 550, 700)
    Mesh* land_mesh = drawGrid(2000.0f, 2000.0f, rasterBandData, min);
    scene->addChild(land_mesh);

    delete rasterBandData;

    if (generateObjFile) {
        generateObj(land_mesh);
    }

    // Add cameras to the scene
    staticCam = new PerspectiveCamera("camera", 1.0f, 3000.0f, 45.0f, 1.0f);
    glm::mat4 s_camera_M = glm::translate(glm::mat4(1.0f), glm::vec3(200.0f, 600.0f, -500.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(240.0f), glm::vec3(0.0f, 1.0f, 0.0f));//* glm::rotate(glm::mat4(1.0f), glm::radians(-2.0f), glm::vec3(1.0f, 0.0f, 1.0f)) )
    //glm::mat4 s_camera_M = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    staticCam->setObjectCoordinates(s_camera_M);

    scene->addChild(staticCam);

    // Parse selected scene and run
    engine->parse(scene);
    engine->run();

    engine->free();
    delete engine;

    return 0;
}