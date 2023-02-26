//////////////
// #INCLUDE //
//////////////

// Library header:
#include "cg_engine.h"

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
        case 'w': // Wireframe toggle
            engine->toggleWireframe();
            break;
        case 'c': // Camera cycle
            engine->cameraRotation();
            break;
        case 'a': // Longer rope
            break;
        case 'q': // Shorter rope
            break;
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

    glm::vec3 n1 = verticies[v1] - verticies[v0];
    glm::vec3 n2 = verticies[v1] - verticies[v2];
    glm::vec3 n = glm::normalize(glm::cross(n1, n2));

    plane->addNormal(n);
    plane->addNormal(n);
    plane->addNormal(n);

    plane->addFace(face);
}

Mesh* drawGrid(float size, int tesselation)
{
    // Compute starting coordinates and step size:
    float start = -size / 2.0f;
    float end = size / 2.0f;
    float triangleSize = size / (float)tesselation;

    // Generate all verticies
    float curZ = start;
    float curX = start;
    while (curZ < end) {
        verticies.push_back(glm::vec3(curX, (float)rand() / RAND_MAX, curZ));
        plane->addVertex(verticies.back());

        curX += triangleSize;

        if (curX >= end) {
            curZ += triangleSize;
            curX = start;
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

/**
 * Application entry point.
 * @param argc number of command-line arguments passed
 * @param argv array containing up to argc passed arguments
 * @return error code (0 on success, error code otherwise)
 */
int main(int argc, char *argv[])
{
    // Init and use the lib:
    CgEngine* engine = CgEngine::getIstance();
    engine->init(argc, argv);

    // Set callbacks
    engine->setKeyboardCallback(keyboardCallback);
    engine->setSpecialCallback(specialCallback);

    // Load scene
    scene = new Node("[root]");

    //TODO: problem with some numbers crashing drawGrid (tesselation examples 550, 700)
    scene->addChild(drawGrid(1000.0f, 2000.0f));
    Light* light = new DirectionalLight("light", 1.0f);
    light->setObjectCoordinates(glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
                                        0.0f, 1.0f, 0.0f, 0.0f,
                                        0.0f, 0.0f, 1.0f, 0.0f,
                                        0.0f, 5.0f, 0.0f, 1.0f));
    scene->addChild(light);

    // Add cameras to the scene
    Camera* staticCam = new PerspectiveCamera("static_cam", 1.0f, 30.0f, 45.0f, 1.0f);
    glm::mat4 s_camera_M = glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 6.5f, 10.0f))
        * glm::rotate(glm::mat4(1.0f), glm::radians(-25.0f), glm::vec3(1.0f, 0.0f, 1.0f))
        * glm::rotate(glm::mat4(1.0f), glm::radians(-40.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    staticCam->setObjectCoordinates(s_camera_M);

    scene->addChild(staticCam);

    // Parse selected scene and run
    engine->parse(scene);
    engine->run();

    engine->free();

    return 0;
}
