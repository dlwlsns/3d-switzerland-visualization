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
    Vertex* v = new Vertex(vertex);
    this->verticies.push_back(v);
}

std::vector<Vertex*> Chunk::getVertecies() {
    return verticies;
}

void Chunk::addEdge(Vertex* start, Vertex* end) {
    this->edges.push_back(new Edge(start, end));
}

void Chunk::addFace(int v0, int v1, int v2) {
    Edge* e1 = new Edge(this->verticies[v0], this->verticies[v1]);
    this->edges.push_back(e1);
    Edge* e2 = new Edge(this->verticies[v1], this->verticies[v2]);
    this->edges.push_back(e2);
    Edge* e3 = new Edge(this->verticies[v2], this->verticies[v0]);
    this->edges.push_back(e3);

    this->edges.end()[-3]->next = this->edges.end()[-2];
    this->edges.end()[-2]->next = this->edges.end()[-1];
    this->edges.end()[-1]->next = this->edges.end()[-3];

    this->faces.push_back(new Face(this->edges.end()[-2]));

    Face* face = this->faces.back();
    glm::vec3 normal = face->getNormal();
    float distance = -glm::dot(normal, face->edge->start->point);
    glm::vec4 plane(normal, distance);


    this->edges.end()[-2]->start->quadric += glm::outerProduct(plane, plane);
    this->edges.end()[-2]->end->quadric += glm::outerProduct(plane, plane);
    this->edges.end()[-2]->next->end->quadric += glm::outerProduct(plane, plane);
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

Chunk* Chunk::create(float size, int tesselation, float** heights, int x, int z)
{
    if (size <= 0.0f || tesselation <= 0 || heights == nullptr) {
        std::cerr << "Invalid input parameters" << std::endl;
        return nullptr;
    }
    Chunk* chunk = new Chunk(x, z);

    // Compute starting coordinates and step size:
    float triangleSize = size / (float)tesselation;

    std::cout << "Generating chunk..." << std::endl;

    // Generate all verticies
    chunk->verticies.reserve(size * size);
    for (int z = 0; z < size; z++) {
        for (int x = 0; x < size; x++) {
            chunk->addVertex(glm::vec3(x * triangleSize, heights[z][x], z * triangleSize));
        }
    }

    // Generate all triangles
    chunk->faces.reserve((size - 1) * (size - 1) * 2);
    for (int y = 0; y < size - 1; y++) {
        for (int x = 0; x < size - 1; x++) {
            chunk->addFace(y * tesselation + x, (y + 1) * tesselation + x, y * tesselation + x + 1);
            chunk->addFace((y + 1) * tesselation + x, (y + 1) * tesselation + x + 1, y * tesselation + x + 1);
        }
    }

    return chunk;
}

// Sort edges from lower error to higher (not deleted before)
struct compare_edges {
    inline bool operator()(const Edge* left, const Edge* right) const {
        if (left->deleted && !right->deleted)
            return false;
        else if (!left->deleted && right->deleted)
            return true;
        else if (!left->deleted && !right->deleted)
            return left->error < right->error;
        else return false;

        //return left->error > right->error;
    }
};

unsigned int deleted = 0;
void Chunk::simplify(unsigned int targetVertexCount) {
    std::cout << "Simplifying... (target: " << targetVertexCount << ")" << std::endl;

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

        // Update the position of the first vertex of the edge
        glm::vec4 newPos = minErrorEdge->findOptimalPosition(minErrorEdge->start->quadric + minErrorEdge->end->quadric);
        minErrorEdge->start->point = glm::vec3(newPos.x, newPos.y, newPos.z);


        // Update the quadric of the first vertex
        minErrorEdge->start->quadric += minErrorEdge->end->quadric;

        // Collapsing the edge
        minErrorEdge->deleted = true;
        minErrorEdge->end->deleted = true;
        
        deleted++;
        count++;

        // Sorting the vector of edges
        if (deleted % 1000000 == 0) {

            for (auto& face : faces) {
                if (face->edge->deleted) {
                    face->edge = face->edge->next;
                }
            }

            // Updating edges error connected to the removed vertex
            for (auto& edge : edges) {
                if (edge->deleted) {
                    continue;
                }

                edge->error = edge->calculateEdgeError();
            }
            std::sort(edges.begin(), edges.end(), compare_edges{});
            count = 0;
        }
    }

    for (auto& face : faces) {
        if (face->edge->deleted) {
            face->edge = face->edge->next;
        }
    }

    // Removing faces
    /*std::vector<Face*> newFaces;
    for (int i = 0; i < faces.size(); i++) {
        Edge* e1 = faces[i]->edge;
        Edge* e2 = e1->next;

        if (e1->deleted || e2->deleted) {
            //delete faces[i];
        }
        else {
            newFaces.push_back(faces[i]);
        }
    }
    faces.clear();
    faces = newFaces;

    
    // Removing verticies
    std::vector<Vertex*> newVertecies;
    int j = 0;
    for (int i = 0; i < verticies.size(); i++) {
        if (verticies[i]->deleted) {
            //delete verticies[i];
            //continue;
        }
        else {
            verticies[i]->id -= j;
            newVertecies.push_back(verticies[i]);
            j++;
        }
    }
    verticies.clear();
    verticies = newVertecies;*/

    deleted = 0;
}
