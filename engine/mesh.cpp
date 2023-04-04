#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <algorithm>

#include "mesh.h"

Mesh::Mesh(char* name) : Node(name) {}

Mesh::~Mesh() {
    std::cout << "Deleted mesh" << std::endl;
}

void Mesh::addVerticies(std::vector<Vertex*> verticies) {
    this->verticies.reserve(verticies.size());
    for (int i = 0; i < verticies.size(); i++) {
        this->verticies.push_back(verticies[i]->point);
    }
}

std::vector<glm::vec3> Mesh::getVerticies() {
    return verticies;
}

void Mesh::addFaces(std::vector<Face*> faces) {
    this->faces.reserve(faces.size());
    for (int i = 0; i < faces.size(); i++) {
        this->faces.push_back(faces[i]->edge->start->id);
        this->faces.push_back(faces[i]->edge->end->id);
        this->faces.push_back(faces[i]->edge->next->end->id);
    }
}
std::vector<unsigned int> Mesh::getFaces() {
    return faces;
}

void Mesh::initVAO()
{
    std::cout << "n verticies: " << verticies.size() << "n faces: " << faces.size() / 3 << std::endl;

    glGenVertexArrays(1, &vaoGlobal);
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

    glBindVertexArray(0);
}

void Mesh::render(glm::mat4 inverseCamera) {
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
}
