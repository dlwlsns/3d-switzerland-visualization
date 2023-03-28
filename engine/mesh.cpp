#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <algorithm>

#include "mesh.h"

Mesh::Mesh(char* name) : Node(name) {}

Mesh::~Mesh() {
    std::cout << "Deleted mesh" << std::endl;
}

void Mesh::addVertex(glm::vec3 vertex) {
    this->verticies.push_back(new Vertex(vertex));
}

std::vector<Vertex *> Mesh::getVertecies() {
    return verticies;
}

void Mesh::addEdge(Vertex* start, Vertex* end) {
    this->edges.push_back(new Edge(start, end));
}

void Mesh::addFace(int v0, int v1, int v2) {
    addEdge(this->verticies[v0], this->verticies[v1]);
    addEdge(this->verticies[v1], this->verticies[v2]);
    addEdge(this->verticies[v2], this->verticies[v0]);

    this->edges.end()[-3]->next = this->edges.end()[-2];
    this->edges.end()[-2]->next = this->edges.end()[-1];
    this->edges.end()[-1]->next = this->edges.end()[-3];

    this->faces.push_back(new Face(this->edges.end()[-2]));
}

void Mesh::initVAO()
{
    //Adding all verticies from a chunk
    for (int i = 0; i < verticies.size(); i++) {
        v.push_back(verticies[i]->point);
    }

    //Adding all faces from a chunk
    for (int i = 0; i < faces.size(); i++) {
        f.push_back(faces[i]->edge->start->id);
        f.push_back(faces[i]->edge->end->id);
        f.push_back(faces[i]->edge->next->end->id);
    }

    std::cout << "n verticies: " << v.size() << "n faces: " << f.size() / 3 << std::endl;

    glGenVertexArrays(1, &vaoGlobal);
    glBindVertexArray(vaoGlobal);

    unsigned int N = v.size();

    glGenBuffers(1, &vboVertex);
    glBindBuffer(GL_ARRAY_BUFFER, vboVertex);
    glBufferData(GL_ARRAY_BUFFER, N * sizeof(glm::vec3), &v[0], GL_STATIC_DRAW);

    glGenBuffers(1, &vboFace);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboFace);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, f.size() * sizeof(unsigned int), &f[0], GL_STATIC_DRAW);

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

    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(vaoGlobal);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(0.0f, 0.0f, 0.0f);
    glDrawElements(GL_TRIANGLES, faces.size() * 3, GL_UNSIGNED_INT, nullptr);
    glEnable(GL_DEPTH_TEST);

    // unbind the vertex array object
    glBindVertexArray(0);
}
