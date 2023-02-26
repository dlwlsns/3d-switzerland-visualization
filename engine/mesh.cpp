#include <iostream>
#include <GL/freeglut.h>

#include "mesh.h"

Mesh::Mesh(char* name) : Node(name), material(nullptr) {}

Mesh::~Mesh() {
    std::cout << "Deleted mesh" << std::endl;
}

Material* Mesh::getMaterial() {
    return material;
}

void Mesh::setMaterial(Material* material) {
    this->material = material;
}

void Mesh::addVertex(glm::vec3 vertex) {
    this->verticies.push_back(vertex);
}

std::vector<glm::vec3> Mesh::getVertecies() {
    return verticies;
}

void Mesh::addNormal(glm::vec3 normal) {
    this->normals.push_back(normal);
}

void Mesh::addFace(unsigned int face[3]) {
    this->faces.push_back(face[0]);
    this->faces.push_back(face[1]);
    this->faces.push_back(face[2]);
}

void Mesh::triangle(int v0, int v1, int v2)
{
    glNormal3f(normals[v0].x, normals[v0].y, normals[v0].z);
    glVertex3f(verticies[v0].x, verticies[v0].y, verticies[v0].z);

    glNormal3f(normals[v1].x, normals[v1].y, normals[v1].z);
    glVertex3f(verticies[v1].x, verticies[v1].y, verticies[v1].z);

    glNormal3f(normals[v2].x, normals[v2].y, normals[v2].z);
    glVertex3f(verticies[v2].x, verticies[v2].y, verticies[v2].z);
}

void Mesh::render(glm::mat4 inverseCamera) {
    //if (this->getMaterial() != nullptr)
    //    this->getMaterial()->render(inverseCamera);

    // Pass a triangle (object coordinates: the triangle is centered around the origin):
    glLoadMatrixf(glm::value_ptr(inverseCamera));

    glBegin(GL_TRIANGLES);
    for (int i = 0; i < faces.size(); i += 3) {
        triangle(faces[i], faces[i + 1], faces[i + 2]);
    }
    glEnd();
}
