#include "chunk.h"

#include <iostream>
#include <algorithm>

Chunk::Chunk(int x, int z) : x(x), z(z) {}

Chunk::~Chunk() {
    empty();

    std::cout << "Deleted chunk" << std::endl;
}

void Chunk::setX(int x) {
    this->x = x;
}

void Chunk::setZ(int z) {
    this->z = z;
}

void Chunk::addVertex(glm::vec3 vertex) {
    vertex.x += (x * 2000);
    vertex.z += (z * 2000);
    this->verticies.push_back(new Vertex(vertex));
}

std::vector<Vertex*> Chunk::getVertecies() {
    return verticies;
}

void Chunk::addEdge(Vertex* start, Vertex* end) {
    this->edges.push_back(new Edge(start, end));
}

void Chunk::addFace(int v0, int v1, int v2) {
    this->edges.push_back(new Edge(this->verticies[v0], this->verticies[v1]));
    this->edges.push_back(new Edge(this->verticies[v1], this->verticies[v2]));
    this->edges.push_back(new Edge(this->verticies[v2], this->verticies[v0]));

    this->edges.end()[-3]->next = this->edges.end()[-2];
    this->edges.end()[-2]->next = this->edges.end()[-1];
    this->edges.end()[-1]->next = this->edges.end()[-3];

    this->faces.push_back(new Face(this->edges.end()[-2]));
}

std::vector<Face*> Chunk::getFaces() {
    return this->faces;
}

void Chunk::empty() {
    while (!verticies.empty()) {
        delete verticies.back();
        verticies.pop_back();
    }

    while (!edges.empty()) {
        delete edges.back();
        edges.pop_back();
    }

    while (!faces.empty()) {
        delete faces.back();
        faces.pop_back();
    }
}