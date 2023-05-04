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
#include <iomanip>
#include "FileDownloader.h"

#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include "json/json.h"
#include <fstream>

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

Geotiff* tiff = new Geotiff();
float** readTiff(const char* path) {
    // create object of Geotiff class
    tiff->Open(path);

    cout << "Reading " << tiff->GetFileName() << endl;

    // dump out Geotiff band NoData value (often it is -9999.0)
    //cout << "No data value: " << tiff->GetNoDataValue() << endl;

    // dump out array (band) dimensions of Geotiff data  
    //int* dims = tiff->GetDimensions();
    //cout << dims[0] << " " << dims[1] << " " << dims[2] << endl;

    // output a value from 2D array  
    float** rasterBandData = tiff->GetRasterBand(1);

    tiff->Close();
    //delete tiff;

    return rasterBandData;
}

Chunk* generateChunk(float size, int tesselation, float** heights, int x, int z)
{
    if (size <= 0.0f || tesselation <= 0 || heights == nullptr) {
        std::cerr << "Invalid input parameters" << std::endl;
        return nullptr;
    }
    Chunk* chunk = new Chunk(x, z);

    // Compute starting coordinates and step size:
    float triangleSize = size / (float)tesselation;

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

struct Tiffile {
    std::string url;
    Json::Value bbox;
    tm date;
    std::string path;

    Tiffile(std::string url, Json::Value bbox, tm date) {
        this->url = url;
        this->bbox = bbox;
        this->date = date;
    }
};

//////////
// MAIN //
//////////

using namespace std;
int main(int argc, char* argv[]) {
    FileDownloader* fd = new FileDownloader();

    std::string base_url = "https://data.geo.admin.ch/api/stac/v0.9/collections/ch.swisstopo.swissalti3d";

    float filter_bbox[2][2];
    //float posx = 0.0;
    //float posy = 0.0;
    //float r = 0.001;

    //Example V. Verzasca
    float posx = 8.85919;
    float posy = 46.22313;
    float r = 0.001;

    /*std::string input;
    bool isValidInput = true;

    std::cout << "Welcome to the 3D Switzerland visualization application, to find the location you want to see use map.geo.admin.ch (coordinates WGS 84)" << std::endl;

    std::cout << "Enter the y coordinate: ";
    std::getline(std::cin, input);
    std::istringstream(input) >> posy;
    if (std::isnan(posy)) {
        std::cout << "Invalid input: y coordinate is not a floating-point number." << std::endl;
        isValidInput = false;
    }

    std::cout << "Enter the x coordinate: ";
    std::getline(std::cin, input);
    std::istringstream(input) >> posx;
    if (std::isnan(posx)) {
        std::cout << "Invalid input: x coordinate is not a floating-point number." << std::endl;
        isValidInput = false;
    }

    std::cout << "Enter the radius: ";
    std::getline(std::cin, input);
    std::istringstream(input) >> r;
    if (std::isnan(r)) {
        std::cout << "Invalid input: radius is not a floating-point number." << std::endl;
        isValidInput = false;
    }

    if (isValidInput) {
        // Do something with the valid input
        std::cout << "The circle with center (" << posx << ", " << posy << ") and radius " << r << " is valid." << std::endl;
    }*/

    // Requesting files from API
    int limit = 100;
    filter_bbox[0][0] = posx - r;
    filter_bbox[0][1] = posy - r;
    filter_bbox[1][0] = posx + r;
    filter_bbox[1][1] = posy + r;
    std::string filter_url = "https://data.geo.admin.ch/api/stac/v0.9/collections/ch.swisstopo.swissalti3d/items?limit=" + std::to_string(limit) + "&bbox=" + std::to_string(filter_bbox[0][0]) + "%2C" + std::to_string(filter_bbox[0][1]) + "%2C" + std::to_string(filter_bbox[1][0]) + "%2C" + std::to_string(filter_bbox[1][1]);
    Json::Value filter_json = fd->JsonParser(fd->MyHTTPRequest(filter_url));
    const Json::Value items = filter_json["features"];
    std::cout << "Items json size: " + std::to_string(items.size()) << std::endl;

    // Filtering old files
    vector<Tiffile*> files;
    for (Json::Value::const_iterator itr = items.begin(); itr != items.end(); itr++) {
        tm time;
        istringstream time_str((*itr)["properties"]["datetime"].asString());
        time_str >> std::get_time(&time, "%Y-%m-%dT%H:%M:%S");
        auto tp1 = std::chrono::system_clock::from_time_t(std::mktime(&time));
        string url = (*itr)["assets"][(*itr)["assets"].getMemberNames()[0]]["href"].asString();

        bool isChunkPresent = false;
        for (auto i = 0; i < files.size(); i++) {
            if (files.at(i)->bbox == (*itr)["bbox"]) {
                isChunkPresent = true;
                auto tp2 = std::chrono::system_clock::from_time_t(std::mktime(&(files.at(i)->date)));
                cout << time_str.str() << " ---  " << std::put_time(&(files.at(i)->date), "%Y-%m-%dT%H:%M:%S") << endl;
                if (tp1 > tp2) {
                    cout << "Replaced " << files.at(i)->url << " with " << url << endl;
                    Tiffile* f = new Tiffile(url, (*itr)["bbox"], time);
                    f->path = "./files/" + FileDownloader::get_file_name(url);
                    files.at(i) = f;
                }
                break;
            }
        }
        if (!isChunkPresent) {
            Tiffile* f = new Tiffile(url, (*itr)["bbox"], time);
            f->path = "./files/" + FileDownloader::get_file_name(url);
            files.push_back(f);
            cout << "Added " << url << " to files" << endl;
        }
    }

    // Downloading files
    vector<string> urls;
    std::vector<float> bboxes;
    for (auto file : files) {
        urls.push_back(file->url);

        bboxes.push_back(file->bbox[0].asFloat());
        bboxes.push_back(file->bbox[1].asFloat());
        bboxes.push_back(file->bbox[2].asFloat());
        bboxes.push_back(file->bbox[3].asFloat());
    }

    fd->filedownloader(urls);
    delete fd;

    

    // Load scene
    scene = new Node("[root]");
    std::vector<Mesh*> ChunkMeshes;
    //scene->addChild(plane);

    string read_center;
    float center_x;
    float center_z;
    float dim_x;
    float dim_z;
    
    Chunk* chunk;

    for (int i = 0; i < bboxes.size()/4; i++) {
        if (posx >= bboxes[i * 4 ] && posy >= bboxes[i * 4 + 1] && posx <= bboxes[i * 4+2] && posy <= bboxes[i * 4+3]) {
            float** rasterBandData = readTiff(files[i]->path.c_str());
            chunk = generateChunk(2000.0f, 2000.0f, rasterBandData, 0, 0);
            delete rasterBandData;
            ChunkMeshes.push_back(new Mesh("0-0"));
            ChunkMeshes.back()->addChunk(chunk);
            scene->addChild(ChunkMeshes.back());
            chunk->empty();

            dim_x = bboxes[i * 4 + 2] - bboxes[i * 4];
            dim_z = bboxes[i * 4 + 3] - bboxes[i * 4 + 1];
            center_x = (bboxes[i * 4 + 2] + bboxes[i * 4]) / 2;
            center_z = (bboxes[i * 4 + 3] + bboxes[i * 4 + 1]) / 2;

            break;
        }
    }
    
    for (int i = 0; i < bboxes.size()/4; i++) {
        if (posx >= bboxes[i * 4] && posy >= bboxes[i * 4 + 1] && posx <= bboxes[i * 4 + 2] && posy <= bboxes[i * 4 + 3]) {
            continue;
        }

        float curr_center_x = (bboxes[i * 4 + 2] + bboxes[i * 4]) / 2;
        float curr_center_z = (bboxes[i * 4 + 3] + bboxes[i * 4 + 1]) / 2;

        float** rasterBandData = readTiff(files[i]->path.c_str());
        chunk = generateChunk(2000.0f, 2000.0f, rasterBandData, round((curr_center_x-center_x)/dim_x), round((center_z- curr_center_z) / dim_z));
        delete rasterBandData;
        ChunkMeshes.push_back(new Mesh("1-1"));
        chunk->simplify(chunk->getVertecies().size() * 0.5f);
        ChunkMeshes.back()->addChunk(chunk);
        scene->addChild(ChunkMeshes.back());
        chunk->empty();
    }

    delete chunk;

    delete tiff;

    cout << "Loaded " << urls.size() << " files" << endl;
    
    // Init and use the lib:
    CgEngine* engine = CgEngine::getIstance();
    engine->init(argc, argv);

    // Set callbacks
    engine->setKeyboardCallback(keyboardCallback);
    engine->setSpecialCallback(specialCallback);

    //plane->initVAO();
    for (auto mesh : ChunkMeshes) {

        mesh->initVAO();
    }

    if (generateObjFile) {
        generateObj(plane);
        cout << "Generated OBJ file" << endl;
    }

    // Add cameras to the scene
    staticCam = new PerspectiveCamera("camera", 1.0f, 3000.0f, 45.0f, 1.0f);
    glm::mat4 s_camera_M = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 600.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(240.0f), glm::vec3(0.0f, 1.0f, 0.0f));//* glm::rotate(glm::mat4(1.0f), glm::radians(-2.0f), glm::vec3(1.0f, 0.0f, 1.0f)) )
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