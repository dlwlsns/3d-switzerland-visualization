//////////////
// #INCLUDE //
//////////////

// Library main include:

// C/C++:
#include <iostream>
#include <vector>

// Glew (include it before GL.h):
#include <GL/glew.h>

// FreeGLUT:   
#include <GL/freeglut.h>
//#include <GLFW/glfw3.h>

// Engine
#include "cg_engine.h"
#include "renderList.h"


//////////////
// DLL MAIN //
//////////////

int windowId;

// Elements
Node* currentScene;
std::vector<Camera*> cameras;
int activeCam = 1;
RenderList* renderlist;

// FPS:
int fps = 0;
int frames = 0;


/////////////////////////////
// BODY OF CLASS cg_engine //
/////////////////////////////

CgEngine* CgEngine::istance = 0;

CgEngine::CgEngine() {
    initFlag = false;
}

CgEngine* CgEngine::getIstance() {
    if (!CgEngine::istance)
        CgEngine::istance = new CgEngine();
    return CgEngine::istance;
}

/**
 * This function allows the client to set a keyboard callback function.
 */
void CgEngine::setKeyboardCallback(void (*func)(unsigned char, int, int)) {
    glutKeyboardFunc(func);
}

/**
 * This function allows the client to set a special callback function.
 */
void CgEngine::setSpecialCallback(void (*func)(int, int, int)) {
    glutSpecialFunc(func);
}

/**
 * This function allows the client to set a idle callback function.
 */
void CgEngine::setIdleCallback(void (*func)()) {
    glutIdleFunc(func);
}

unsigned int CgEngine::getElapsedTime() {
    return glutGet(GLUT_ELAPSED_TIME);
}

/**
 * This function add the node and all his children to the RenderList.
 *
 * @param node node to add
 */
void CgEngine::parse(Node* scene) {
    if (scene->getParent() == nullptr) {
        while (cameras.size() > 1)
        {
            cameras.pop_back();
        }

        currentScene = scene;
    }

    if (dynamic_cast<const Camera*>(scene) != nullptr) {
        cameras.push_back(dynamic_cast<Camera*>(scene));
    }
    else {
        renderlist->addItem(RenderItem(scene, scene->getWorldCoordinates()));
    }

    for (int i = 0; i < scene->getChildrenCount(); i++) {
        parse(scene->getChild(i));
    }

    renderlist->sort();
}

/**
 * This method start the render of the scene.
 */
void CgEngine::run() {
    // Enter the main FreeGLUT processing loop:
    if (renderlist->size() == 0) {
        std::cout << "Scene not loaded." << std::endl;
        return;
    }

    if (cameras.size() == 1) {
        std::cout << "No camera loaded." << std::endl;
        return;
    }

    glutMainLoop();
}

/**
 * Deinitialization method.
 * @return true on success, false on error
 */
bool CgEngine::free()
{
    // Really necessary?
    if (!initFlag)
    {
        std::cout << "ERROR: class not initialized" << std::endl;
        return false;
    }

    delete currentScene;

    while (!cameras.empty()) {
        cameras.pop_back();
    }

    delete renderlist;

    // Done:
    initFlag = false;
    return true;
}


///////////////
// CALLBACKS //
///////////////

/**
 * This callback is invoked each second.
 * 
 * @param value passepartout value
 */
void timerCallback(int value)
{
    // Update values:
    fps = frames;
    frames = 0;

    // Register the next update:
    glutTimerFunc(1000, timerCallback, 0);
}

/**
 * This is the main rendering routine automatically invoked by FreeGLUT.
 */
void displayCallback()
{
    // Clear the screen:         
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f); // RGBA components
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearDepth(1.0f);

    glMatrixMode(GL_MODELVIEW);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Render Nodes
    CgEngine::getIstance()->parse(currentScene);
    renderlist->render(cameras[activeCam]->getInverse());

    //////////////////////////
   // Switch to 2D rendering:

   // Set orthographic projection:
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(cameras[0]->getProjection()));
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(glm::mat4(1.0f)));

    glColor3f(1.0f, 0.0f, 0.0f);

    // Write some text:
    char text[64];
    sprintf(text, "FPS: %d", fps);
    glRasterPos2f(1.0f, 2.0f);
    glutBitmapString(GLUT_BITMAP_8_BY_13, (unsigned char*)text);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(glm::value_ptr(cameras[activeCam]->getProjection()));

    // Swap this context's buffer:  
    frames++;
    glutSwapBuffers();

    // Force rendering refresh:
    glutPostWindowRedisplay(windowId);
}

/**
 * This callback is invoked each time the window gets resized (and once also when created).
 * 
 * @param width new window width
 * @param height new window height
 */
void reshapeCallback(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);

    for (int i = 0; i < cameras.size(); i++) {
        if (i == 0) {
            OrthographicCamera* gui = dynamic_cast<OrthographicCamera*>(cameras[i]);

            gui->setXMax(width);
            gui->setYMax(height);

            gui->setProjection(glm::ortho(gui->getXMin(), gui->getXMax(), gui->getYMin(), gui->getYMax(), gui->getNearPlane(), gui->getFarPlane()));
            continue;
        }

        if (dynamic_cast<const PerspectiveCamera*>(cameras[i]) != nullptr) {
            PerspectiveCamera* pCamera = dynamic_cast<PerspectiveCamera*>(cameras[i]);

            pCamera->setAspectRatio((float)width / (float)height);

            pCamera->setProjection(glm::perspective(glm::radians(pCamera->getFov()), pCamera->getAspectRatio(), pCamera->getNearPlane(), pCamera->getFarPlane()));
        }
        else {
            OrthographicCamera* oCamera = dynamic_cast<OrthographicCamera*>(cameras[i]);
            oCamera->setXMin(width/-200.0f);
            oCamera->setYMin(height/-200.0f);
            oCamera->setXMax(width/200.0f);
            oCamera->setYMax(height/200.0f);

            oCamera->setProjection(glm::ortho(oCamera->getXMin(), oCamera->getXMax(), oCamera->getYMin(), oCamera->getYMax(), oCamera->getNearPlane(), oCamera->getFarPlane()));
        }
    }

    glLoadMatrixf(glm::value_ptr(cameras[activeCam]->getProjection()));
    
    glMatrixMode(GL_MODELVIEW);
}


///////////
// UTILS //
///////////

/**
 * This method print all the OpenGL details for the current machine.
 */
void getGlDetails() {
    // Check OpenGL version:
    std::cout << "OpenGL context" << std::endl;
    std::cout << "   version  . . : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "   vendor . . . : " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "   renderer . . : " << glGetString(GL_RENDERER) << std::endl;

    // Additional context info:
    std::cout << "   color bit  . : " << glutGet(GLUT_WINDOW_BUFFER_SIZE)
        << " (red: " << glutGet(GLUT_WINDOW_RED_SIZE)
        << ", green: " << glutGet(GLUT_WINDOW_GREEN_SIZE)
        << ", blue: " << glutGet(GLUT_WINDOW_BLUE_SIZE)
        << ", alpha: " << glutGet(GLUT_WINDOW_ALPHA_SIZE)
        << ")" << std::endl;
    int bits = 0;
    glGetIntegerv(GL_DEPTH_BITS, &bits);
    std::cout << "   depth bit  . : " << glutGet(GLUT_WINDOW_DEPTH_SIZE) << " (max " << bits << ")" << std::endl;
    std::cout << "   stencil bit  : " << glutGet(GLUT_WINDOW_STENCIL_SIZE) << std::endl;
    std::cout << "   double buff. : " << glutGet(GLUT_WINDOW_DOUBLEBUFFER) << std::endl;
}

/**
 * Initialization method.
 * @return true on success, false on error
 */
bool CgEngine::init(int argc, char* argv[])
{
    // Prevent double init:
    if (initFlag)
    {
        std::cout << "ERROR: class already initialized" << std::endl;
        return false;
    }

    // FreeGLUT can parse command-line params, in case:
    glutInit(&argc, argv);

    // Init and use the lib:
    // Init context:
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(1280, 720);
    glutTimerFunc(1000, timerCallback, 0);

    // Set some optional flags:
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    // Create the window with a specific title:   
    windowId = glutCreateWindow("3D Switzerland Visualization");

    // Init Glew (*after* the context creation):
    glewExperimental = GL_TRUE;
    glewInit();

    // OpenGL 2.1 is required:
    if (!glewIsSupported("GL_VERSION_2_1"))
    {
        std::cout << "OpenGL 2.1 not supported" << std::endl;
        return 0;
    }

    // The OpenGL context is now initialized...
    glEnable(GL_DEPTH_TEST);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    // Set lighting options
    //glEnable(GL_LIGHTING);
    glEnable(GL_CULL_FACE);
    //glEnable(GL_NORMALIZE);

    //glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0f);

    // Set callback functions:
    glutDisplayFunc(displayCallback);
    glutReshapeFunc(reshapeCallback);

    getGlDetails();

    renderlist = new RenderList("renderlist");

    // Add an Orthographic Camera for the GUI
    Camera* gui = new OrthographicCamera("GUI", -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f);
    cameras.push_back(gui);

    // Done:
    initFlag = true;

    return true;
}
