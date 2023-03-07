//////////////
// #INCLUDE //
//////////////

// Library header:
#include "cg_engine.h"

#include "tiffio.h"
//#include "Geotiff.cpp"

// C/C++:
#include <iostream>
#include <vector>

CgEngine* engine;
Node* scene;

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
    switch (key)
    {
        case 's': //hook/unhook objects
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
            break;
        case 102://right
            break;
        case 101://up
            break;
        case 103://down
            break;
        default:
            break;
    }
}

Mesh* plane = new Mesh("plane");
std::vector<glm::vec3> verticies;

void face(int v0, int v1, int v2) {
    unsigned int face[3] = { v0, v1, v2 };

    //glm::vec3 n1 = verticies[v1] - verticies[v0];
    //glm::vec3 n2 = verticies[v1] - verticies[v2];
    //glm::vec3 n = glm::normalize(glm::cross(n1, n2));

    plane->addFace(face);
}

Mesh* drawGrid(float size, int tesselation, float** heights, float min)
{
    // Compute starting coordinates and step size:
    float start = -size / 2.0f;
    float end = size / 2.0f;
    float triangleSize = size / (float)tesselation;

    // Generate all verticies
    float curZ = start;
    float curX = start;

    int x = 0;
    int y = 0;

    while (curZ < end) {
        verticies.push_back(glm::vec3(curX, heights[x][y], curZ));//heights[x][y]-min      rand() % 10
        //std::cout << heights[x][y] << std::endl;
        plane->addVertex(verticies.back());

        curX += triangleSize;
        x++;

        if (curX >= end) {
            curZ += triangleSize;
            curX = start;

            y++;
            x = 0;
        }
    }

    // Generate all triangles
    for (int i = 0; i < verticies.size(); i++) {
        if (i < verticies.size() - tesselation) {
            if (i % tesselation < tesselation - 1) {
                face(i, i + tesselation, i + 1);
                face(i + tesselation, i + tesselation + 1, i+1);
            }
        }
    }

    std::cout << "verticies count: " << verticies.size() << std::endl;
    return plane;
}

//////////
// MAIN //
//////////

#include "iostream"
#include "stdlib.h"
#include "string.h"
#include "Geotiff.cpp"
using namespace std;
int main(int argc, char* argv[]) {
    // create object of Geotiff class
    Geotiff tiff((const char*)"../swissalti3d_2022_2709-1119_0.5_2056_5728.tif");

    cout << tiff.GetFileName() << endl;

    // dump out Geotiff band NoData value (often it is -9999.0)
    cout << "No data value: " << tiff.GetNoDataValue() << endl;

    // dump out array (band) dimensions of Geotiff data  
    int* dims = tiff.GetDimensions();
    cout << dims[0] << " " << dims[1] << " " << dims[2] << endl;

    // output a value from 2D array  
    float** rasterBandData = tiff.GetRasterBand(1);

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



    // Init and use the lib:
    CgEngine* engine = CgEngine::getIstance();
    engine->init(argc, argv);

    // Set callbacks
    engine->setKeyboardCallback(keyboardCallback);
    engine->setSpecialCallback(specialCallback);

    // Load scene
    scene = new Node("[root]");

    //TODO: problem with some numbers crashing drawGrid (tesselation examples 550, 700)
    scene->addChild(drawGrid(2000.0f, 2000.0f, rasterBandData, min));

    // Add cameras to the scene
    Camera* staticCam = new PerspectiveCamera("camera", 1.0f, 3000.0f, 45.0f, 1.0f);
    glm::mat4 s_camera_M = glm::translate(glm::mat4(1.0f), glm::vec3(-50.0f, 500.0f, 50.0f))
        * glm::rotate(glm::mat4(1.0f), glm::radians(-2.0f), glm::vec3(1.0f, 0.0f, 1.0f))
        * glm::rotate(glm::mat4(1.0f), glm::radians(220.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    staticCam->setObjectCoordinates(s_camera_M);

    scene->addChild(staticCam);

    // Parse selected scene and run
    engine->parse(scene);
    engine->run();

    engine->free();

    return 0;
}