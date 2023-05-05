//////////////
// #INCLUDE //
//////////////

// Library header:
#include "cg_engine.h"
#include "geotiff.h"

// C/C++:
#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <chrono>
#include <iomanip>

#include "fileDownloader.h"
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include "json/json.h"

CgEngine* engine;
Node* scene;
Camera* staticCam;

bool generateObjFile = false;

/**
 * Search a node by name inside a scene tree
 */
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
        // Camera movement
        case 'w':
            staticCam->appendMatrix(glm::inverse(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 10.0f))));
            break;
        case 's':
            staticCam->appendMatrix(glm::inverse(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -10.0f))));
            break;
        case 'a':
            staticCam->appendMatrix(glm::inverse(glm::translate(glm::mat4(1.0f), glm::vec3(10.0f, 0.0f, 0.0f))));
            break;
        case 'd':
            staticCam->appendMatrix(glm::inverse(glm::translate(glm::mat4(1.0f), glm::vec3(-10.0f, 0.0f, 0.0f))));
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
    // Camera rotation
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

/**
 * Read from a TIFF file the first image
 */
Geotiff* tiff = new Geotiff();
float** readTiff(const char* path) {
    // create object of Geotiff class
    tiff->Open(strdup(path));

    std::cout << "Reading " << tiff->GetFileName() << std::endl;

    // output a value from 2D array  
    float** rasterBandData = tiff->GetRasterBand(1);

    tiff->Close();

    return rasterBandData;
}

void generateObj(Mesh* mesh) {
    auto now = std::chrono::system_clock::now();
    auto UTC = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
    std::ofstream objfile("./files/switzerland_" + std::to_string(UTC) + ".obj");

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
    String url;
    Json::Value bbox;
    tm date;
    String path;

    Tiffile(String url, Json::Value bbox, tm date) {
        this->url = url;
        this->bbox = bbox;
        this->date = date;
    }
};

//////////
// MAIN //
//////////

int main(int argc, char* argv[]) {
    FileDownloader* fd = new FileDownloader();

    std::string base_url = "https://data.geo.admin.ch/api/stac/v0.9/collections/ch.swisstopo.swissalti3d";

    float filter_bbox[2][2];

    //Example Lugano
    //float posx = 8.96166;
    //float posy = 46.00782;
    //float r = 0.01;

    float posx = 0.0;
    float posy = 0.0;
    float r = 0.001;

    std::string input;
    bool isValidInput = true;

    std::cout << "Welcome to the 3D Switzerland visualization application, to find the location you want to see use https://map.geo.admin.ch (coordinates WGS 84)" << std::endl;

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

    if (!isValidInput) {
        std::cout << "ERROR: Invalid " << std::endl;
        exit(1);
    }

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
    std::vector<Tiffile*> files;
    for (Json::Value::const_iterator itr = items.begin(); itr != items.end(); itr++) {
        tm time;
        std::istringstream time_str((*itr)["properties"]["datetime"].asString());
        time_str >> std::get_time(&time, "%Y-%m-%dT%H:%M:%S");
        auto tp1 = std::chrono::system_clock::from_time_t(std::mktime(&time));
        String url = (*itr)["assets"][(*itr)["assets"].getMemberNames()[0]]["href"].asString();

        bool isChunkPresent = false;
        for (auto i = 0; i < files.size(); i++) {
            if (files.at(i)->bbox == (*itr)["bbox"]) {
                isChunkPresent = true;
                auto tp2 = std::chrono::system_clock::from_time_t(std::mktime(&(files.at(i)->date)));
                //cout << time_str.str() << " ---  " << std::put_time(&(files.at(i)->date), "%Y-%m-%dT%H:%M:%S") << endl;
                if (tp1 > tp2) {
                    std::cout << "Replaced " << files.at(i)->url << " with " << url << std::endl;
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
            std::cout << "Added " << url << " to files" << std::endl;
        }
    }

    // Downloading files
    std::vector<String> urls;
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

    String read_center;
    float center_x;
    float center_z;
    float dim_x;
    float dim_z;
    
    Chunk* chunk;

    // Generating central chunk
    for (int i = 0; i < bboxes.size()/4; i++) {
        if (posx >= bboxes[i * 4 ] && posy >= bboxes[i * 4 + 1] && posx <= bboxes[i * 4+2] && posy <= bboxes[i * 4+3]) {
            float** rasterBandData = readTiff(files[i]->path.c_str());
            chunk = Chunk::create(2000.0f, 2000.0f, rasterBandData, 0, 0);
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
    
    // Generating other chunks
    for (int i = 0; i < bboxes.size()/4; i++) {
        if (posx >= bboxes[i * 4] && posy >= bboxes[i * 4 + 1] && posx <= bboxes[i * 4 + 2] && posy <= bboxes[i * 4 + 3]) {
            continue;
        }

        float curr_center_x = (bboxes[i * 4 + 2] + bboxes[i * 4]) / 2;
        float curr_center_z = (bboxes[i * 4 + 3] + bboxes[i * 4 + 1]) / 2;

        float** rasterBandData = readTiff(files[i]->path.c_str());
        chunk = Chunk::create(2000.0f, 2000.0f, rasterBandData, round((curr_center_x-center_x)/dim_x), round((center_z- curr_center_z) / dim_z));
        delete rasterBandData;
        ChunkMeshes.push_back(new Mesh("1-1"));
        chunk->simplify(chunk->getVertecies().size() * 0.8f);
        ChunkMeshes.back()->addChunk(chunk);
        scene->addChild(ChunkMeshes.back());
        chunk->empty();
    }

    delete chunk;

    delete tiff;

    std::cout << "Loaded " << urls.size() << " files" << std::endl;
    
    // Init and use the engine
    CgEngine* engine = CgEngine::getIstance();
    engine->init(argc, argv);

    // Set callbacks
    engine->setKeyboardCallback(keyboardCallback);
    engine->setSpecialCallback(specialCallback);

    for (auto mesh : ChunkMeshes) {
        mesh->initVAO();

        if (generateObjFile) {
            generateObj(mesh);
            std::cout << "Generated OBJ file" << std::endl;
        }
    }

    // Add camera to the scene
    staticCam = new PerspectiveCamera("camera", 1.0f, 3000.0f, 45.0f, 1.0f);
    glm::mat4 s_camera_M = glm::translate(glm::mat4(1.0f), glm::vec3(100.0f, 600.0f, 1000.0f)) * glm::rotate(glm::mat4(1.0f), glm::radians(240.0f), glm::vec3(0.0f, 1.0f, 0.0f));//* glm::rotate(glm::mat4(1.0f), glm::radians(-2.0f), glm::vec3(1.0f, 0.0f, 1.0f)) )
    staticCam->setObjectCoordinates(s_camera_M);

    scene->addChild(staticCam);

    std::cout << "Added camera" << std::endl;

    // Parse selected scene and run
    engine->parse(scene);
    engine->run();

    engine->free();
    delete engine;

    return 0;
}