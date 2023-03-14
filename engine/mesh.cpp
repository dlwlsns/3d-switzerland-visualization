#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "mesh.h"

Mesh::Mesh(char* name) : Node(name) {}

Mesh::~Mesh() {
    std::cout << "Deleted mesh" << std::endl;
}

void Mesh::addVertex(glm::vec3 vertex) {
    this->verticies.push_back(vertex);
}

std::vector<glm::vec3> Mesh::getVertecies() {
    return verticies;
}

void Mesh::addFace(unsigned int face[3]) {
    this->faces.push_back(face[0]);
    this->faces.push_back(face[1]);
    this->faces.push_back(face[2]);
}

void Mesh::initVAO()
{
    glGenVertexArrays(1, &vaoGlobal);
    glBindVertexArray(vaoGlobal);

    unsigned int N = verticies.size();
    unsigned char* color = nullptr;
    color = new unsigned char[3 * N];
    for (int c = 0; c < 3 * N; c++)
        color[c] = 255;

    glGenBuffers(1, &vboVertex);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertex);
    glBufferData(GL_ARRAY_BUFFER, N * sizeof(glm::vec3), &verticies[0], GL_STATIC_DRAW);

    glGenBuffers(1, &vboColor);
    glBindBuffer(GL_ARRAY_BUFFER, vboColor);
    glBufferData(GL_ARRAY_BUFFER, N * 3 * sizeof(unsigned char), color, GL_STATIC_DRAW);

    glGenBuffers(1, &vboFace);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboFace);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, faces.size() * sizeof(unsigned int), &faces[0], GL_STATIC_DRAW);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, vboVertex);
    glVertexPointer(3, GL_FLOAT, 0, nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, vboColor);
    glColorPointer(3, GL_UNSIGNED_BYTE, 0, nullptr);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboFace);

    glBindVertexArray(0);
}

void Mesh::triangle(int v0, int v1, int v2)
{
    glVertex3f(verticies[v0].x, verticies[v0].y, verticies[v0].z);
    glVertex3f(verticies[v1].x, verticies[v1].y, verticies[v1].z);
    glVertex3f(verticies[v2].x, verticies[v2].y, verticies[v2].z);
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
    // Pass a triangle (object coordinates: the triangle is centered around the origin):
    glLoadMatrixf(glm::value_ptr(inverseCamera));
    glDepthFunc(GL_LESS);

    glBindVertexArray(vaoGlobal);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glColor3f(1.0f, 1.0f, 1.0f);
    glDrawElements(GL_TRIANGLES, faces.size() * 3, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(vaoGlobal);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(0.0f, 0.0f, 0.0f);
    glDrawElements(GL_TRIANGLES, faces.size() * 3, GL_UNSIGNED_INT, nullptr);
    glEnable(GL_DEPTH_TEST);

    // unbind the vertex array object
    glBindVertexArray(0);
}
