#include "chunk.h"

#include <iostream>
#include <algorithm>
#include <list>

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

    Face* face = this->faces.back();
    glm::vec3 normal = face->getNormal();
    float distance = -glm::dot(normal, face->edge->start->point);
    glm::vec4 plane(normal, distance);


    this->edges.end()[-3]->start->quadric += glm::outerProduct(plane, plane);
    this->edges.end()[-3]->end->quadric += glm::outerProduct(plane, plane);
    this->edges.end()[-3]->next->end->quadric += glm::outerProduct(plane, plane);
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

// Sort edges from lower error to higher (not deleted before)
struct compare_edges {
    inline bool operator()(const Edge* left, const Edge* right) const {
        /*if (left->deleted && !right->deleted)
            return true;
        else if (!left->deleted && right->deleted)
            return false;
        else if (!left->deleted && !right->deleted)
            return left->error < right->error;
        else return false;*/

        return left->error > right->error;
    }
};

unsigned int deleted = 0;
void Chunk::simplify(unsigned int targetVertexCount) {
    std::cout << "Target vertex count: " << targetVertexCount << std::endl;

    std::sort(edges.begin(), edges.end(), compare_edges{});

    // Simplify the mesh until the target vertex count is reached
    Edge* minErrorEdge;
    int count = 0;
    while (verticies.size()-deleted > targetVertexCount) {
        
        // Find the edge with the lowest error        
        minErrorEdge = edges[count];

        if (minErrorEdge->deleted || minErrorEdge->start->deleted || minErrorEdge->end->deleted) {
            minErrorEdge->deleted = true;

            count++;
            continue;
        }
        else {
            if (deleted % 100000 == 0)
                std::cout << "Cycling... Size:" << verticies.size() - deleted << std::endl;
        }

        // Update the position of the first vertex of the edge
        minErrorEdge->start->point = glm::vec3(minErrorEdge->findOptimalPosition(minErrorEdge->start->quadric + minErrorEdge->end->quadric));


        // Update the quadric of the first vertex
        minErrorEdge->start->quadric += minErrorEdge->end->quadric;

        // Collapsing the edge
        minErrorEdge->deleted = true;
        minErrorEdge->end->deleted = true;
        //minErrorEdge->end = minErrorEdge->start;
        //minErrorEdge->next->start = minErrorEdge->start;

        // Updating edges connected to the removed vertex
        for (auto& edge : minErrorEdge->end->edges) {
            if (edge->deleted) {
                continue;
            }

            if (edge->start == minErrorEdge->end) {
                //edge->start = minErrorEdge->start;
            }
            else {
                //edge->end = minErrorEdge->start;
            }

            edge->error = edge->calculateEdgeError();
        }

        deleted++;
        count++;

        // Sorting the vector of edges
        if (deleted % 1000000 == 0) {
            std::sort(edges.begin(), edges.end(), compare_edges{});
            count = 0;
        }
    }

    /*std::cout << "Removing faces.." << std::endl;
    std::vector<Face*> newFaces;
    for (int i = 0; i < faces.size(); i++) {
        Edge* e1 = faces[i]->edge;
        Edge* e2 = e1->next;

        if (e1->deleted || e2->deleted) {
            delete faces[i];
            continue;
        }
        
        newFaces.push_back(faces[i]);
    }
    faces.clear();
    faces = newFaces;*/

    /*std::cout << "Removing vertices.." << std::endl;
    std::vector<Vertex*> newVertecies;
    for (int i = 0; i < verticies.size(); i++) {
        if (verticies[i]->deleted) {
            delete verticies[i];
            continue;
        }

        verticies[i]->id -= deleted;
        newVertecies.push_back(verticies[i]);
    }*/

    deleted = 0;
}
