#include <iostream>
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

void Mesh::triangle(int v0, int v1, int v2)
{
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
    glEnd();
}

void Mesh::render(glm::mat4 inverseCamera) {
    // Pass a triangle (object coordinates: the triangle is centered around the origin):
    glLoadMatrixf(glm::value_ptr(inverseCamera));
    
    for (int i = 0; i < faces.size(); i += 3) {
        triangle(faces[i], faces[i + 1], faces[i + 2]);
    }
    
}
