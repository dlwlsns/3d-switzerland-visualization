#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "mesh.h"

Mesh::Mesh(char* name) : Node(name) {}

Mesh::~Mesh() {
    std::cout << "Deleted mesh" << std::endl;
}

void Mesh::addVertex(glm::vec3 vertex) {
    this->vertices.push_back(vertex);
}

std::vector<glm::vec3> Mesh::getVertecies() {
    return vertices;
}
std::vector<unsigned int> Mesh::getFaces() {
    return faces;
}

void Mesh::addFace(unsigned int face[3]) {
    this->faces.push_back(face[0]);
    this->faces.push_back(face[1]);
    this->faces.push_back(face[2]);
}

void Mesh::initVAO()
{
    generateLods(); // Generate LOD levels first

    // Generate and bind vertex arrays and buffers for each level of detail
    unsigned int numLods = vaoLevels.size(); // Get the number of LOD levels
    vboVertexLevels.resize(numLods);
    vboFaceLevels.resize(numLods);

    glGenBuffers(numLods, &vboVertexLevels[0]);
    glGenBuffers(numLods, &vboFaceLevels[0]);

    for (unsigned int i = 0; i < numLods; i++)
    {
        glBindVertexArray(vaoLevels[i]);

        unsigned int N = verticesLevels[i].size(); // Get the number of vertices for the current LOD level

        glBindBuffer(GL_ARRAY_BUFFER, vboVertexLevels[i]);
        glBufferData(GL_ARRAY_BUFFER, N * sizeof(glm::vec3), &verticesLevels[i][0], GL_STATIC_DRAW); // Use verticesLevels[i] instead of vertices

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboFaceLevels[i]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, facesLevels[i].size() * sizeof(unsigned int), &facesLevels[i][0], GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindVertexArray(0);
    }
    
    /*glGenVertexArrays(1, &vaoGlobal);
    glBindVertexArray(vaoGlobal);

    unsigned int N = verticies.size();

    glGenBuffers(1, &vboVertex);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertex);
    glBufferData(GL_ARRAY_BUFFER, N * sizeof(glm::vec3), &verticies[0], GL_STATIC_DRAW);

    glGenBuffers(1, &vboFace);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboFace);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(unsigned int), &faces[0], GL_STATIC_DRAW);

    glEnableClientState(GL_VERTEX_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, vboVertex);
    glVertexPointer(3, GL_FLOAT, 0, nullptr);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboFace);

    glBindVertexArray(0);*/
}

void Mesh::triangle(int v0, int v1, int v2)
{
    //glVertex3f(verticies[v0].x, verticies[v0].y, verticies[v0].z);
    //glVertex3f(verticies[v1].x, verticies[v1].y, verticies[v1].z);
    //glVertex3f(verticies[v2].x, verticies[v2].y, verticies[v2].z);
    /*
    glBegin(GL_TRIANGLES);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glVertex3f(verticies[v0].x, verticies[v0].y, verticies[v0].z);

        glVertex3f(verticies[v1].x, verticies[v1].y, verticies[v1].z);

        glVertex3f(verticies[v2].x, verticies[v2].y, verticies[v2].z);
    glEnd();

    glBegin(GL_LINES);
    glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
        glVertex3f(verticies[v0].x, verticies[v0].y, verticies[v0].z);
        glVertex3f(verticies[v1].x, verticies[v1].y, verticies[v1].z);

        glVertex3f(verticies[v0].x, verticies[v0].y, verticies[v0].z);
        glVertex3f(verticies[v2].x, verticies[v2].y, verticies[v2].z);

        glVertex3f(verticies[v2].x, verticies[v2].y, verticies[v2].z);
        glVertex3f(verticies[v1].x, verticies[v1].y, verticies[v1].z);
    glEnd();*/
}

void Mesh::render(glm::mat4 inverseCamera) {
    // Calculate camera position in object space
    glm::vec3 cameraPosition = glm::inverse(inverseCamera)[3];

    // Render each triangle with the appropriate LOD level
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        glm::vec3 v0 = vertices[faces[i]];
        glm::vec3 v1 = vertices[faces[i + 1]];
        glm::vec3 v2 = vertices[faces[i + 2]];

        float distance = glm::min(glm::distance(cameraPosition, v0),
            glm::min(glm::distance(cameraPosition, v1),
                glm::distance(cameraPosition, v2)));

        // Determine the current LOD level based on the triangle's distance from the camera
        int currentLod = 0; // Set the default LOD level to the highest level of detail
        for (unsigned int j = 0; j < lodDistances.size(); j++)
        {
            if (distance < lodDistances[j])
            {
                currentLod = j; // Update the LOD level
                break;
            }
        }

        // Bind the appropriate VAO for the current LOD level
        glBindVertexArray(vaoLevels[currentLod]);

        // Render the triangle
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(i * 3 * sizeof(unsigned int)));

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glColor3f(1.0f, 1.0f, 1.0f);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(i * 3 * sizeof(unsigned int)));
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(1.0f);
        glColor3f(0.0f, 0.0f, 0.0f);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(i * 3 * sizeof(unsigned int)));
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    glBindVertexArray(0);


    /* OLD
    // Pass a triangle (object coordinates: the triangle is centered around the origin):
    glLoadMatrixf(glm::value_ptr(inverseCamera));
    glDepthFunc(GL_LESS);
    glBindVertexArray(vaoGlobal);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor3f(1.0f, 1.0f, 1.0f);
    glDrawElements(GL_TRIANGLES, faces.size() * 3, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    glBindVertexArray(vaoGlobal);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(1.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    glDrawElements(GL_TRIANGLES, faces.size() * 3, GL_UNSIGNED_INT, nullptr);
    glClear(GL_DEPTH_BUFFER_BIT);
    // unbind the vertex array object
    glBindVertexArray(0);
    */
}

void Mesh::setLODDistances(const std::vector<float>& distances)
{
    lodDistances = distances;
}

void Mesh::generateLods() {
    // Clear existing LOD levels
    vaoLevels.clear();
    verticesLevels.clear();
    facesLevels.clear();

    // Iterate over LOD levels
    for (size_t i = 0; i < lodFactors.size(); ++i) {

        // Simplify mesh
        simplify_mesh(i);

        // Convert simplified vertices and indices to vector of glm::vec3 and unsigned int
        /*std::vector<glm::vec3> simplifiedVerticesGLM;
        for (const auto& vertex : simplifiedVertices) {
            simplifiedVerticesGLM.push_back(vertex.position);
        }
        for (const auto& index : simplifiedIndices) {
            simplifiedIndicesGLM.push_back(index);
        }*/

        // Create VAO and IBO for the LOD level
        GLuint vao;
        glGenVertexArrays(1, &vao);

        glBindVertexArray(vao);

        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        //glBufferData(GL_ELEMENT_ARRAY_BUFFER, simplifiedIndicesGLM.size() * sizeof(unsigned int), &simplifiedIndicesGLM[0], GL_STATIC_DRAW);

        //glBindBuffer(GL_ARRAY_BUFFER, vbo);
        //glBufferData(GL_ARRAY_BUFFER, simplifiedVerticesGLM.size() * sizeof(glm::vec3), &simplifiedVerticesGLM[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Store VAO, vertices and indices in LOD levels vectors
        vaoLevels.push_back(vao);
        //verticesLevels.push_back(simplifiedVerticesGLM);
        //facesLevels.push_back(simplifiedIndicesGLM);
    }
}

void Mesh::simplify_mesh(unsigned int targetLod)
{
    while (vaoLevels.size() > targetLod + 1) {
        // Perform edge collapse to simplify the mesh
        for (unsigned int i = 0; i < faces.size(); i++) {
            // Compute the cost of collapsing the edge associated with the current triangle
            // (Note: this is a simplified cost function, actual cost function may be more complex)
            float cost = compute_collapse_cost(faces[i]);

            // If the cost is below a threshold, perform the edge collapse
            if (cost < 1) {                                                                             //FIXME: should be something like this -> "collapseThreshold) {"
                // Perform the edge collapse
                perform_edge_collapse(faces[i]);

                // Remove the collapsed triangle from the mesh
                faces.erase(faces.begin() + i);
                i--;

                // Update the vertex arrays and buffers
                //update_vertex_arrays_and_buffers();                                                     //TODO
            }
        }
    }
}

void Mesh::perform_edge_collapse(int faceIndex) {
    vertices[faceIndex + 1] = vertices[faceIndex]; // Strange stuff

    faces.erase(faces.begin() + faceIndex + 2);
    faces.erase(faces.begin() + faceIndex + 1);
    faces.erase(faces.begin() + faceIndex);
}

float Mesh::compute_collapse_cost(int faceIndex) {
    std::vector<int> tu;
    std::vector<int> tuv;

    glm::vec3 u = vertices[faces[faceIndex]];
    glm::vec3 v = vertices[faces[faceIndex + 1]];

    for (int i = 0; i < faces.size(); i = i + 3)
    {
        for (int j = 0; j < 3; j++) {
            if (vertices[faces[i]] == u) {
                tu.push_back(i);
                continue;
            }

            if (vertices[faces[i]] == v && (tu.back() == (i - 1 || i - 2))) {
                tuv.push_back(i);
                continue;
            }
        }
    }

    float edgelength = glm::length(v - u);
    float curvature = 0;
    for (int i = 0; i < tu.size(); i++) {
        float mincurv = 1; // curve for face i and closer side to it
        for (int j = 0; j < tuv.size(); j++) {
            // use dot product of face normals. '^' defined in vector

            float dotprod = glm::dot(glm::cross(vertices[faces[tu[i]]], vertices[faces[tu[i + 1]]]), glm::cross(vertices[faces[tuv[j]]], vertices[faces[tuv[j + 1]]]));
            mincurv = std::min(mincurv, (1 - dotprod) / 2.0f);
        }
        curvature = std::max(curvature, mincurv);
    }

    return edgelength * curvature;
}
