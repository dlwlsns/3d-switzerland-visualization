#include "chunk.h"
#include "PMEdge.h"
#include "PMVertex.h"

#include <iostream>
#include <algorithm>
#include <map>
#include <queue>

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

void Chunk::simplify(unsigned int targetVertexCount) {
    std::cout << "Target vertex count: " << targetVertexCount << std::endl;
    std::vector<PMVertex*> pmVertices;
    std::vector<PMEdge*> pmEdges;

    std::cout << "Start initializePMData" << std::endl;
    initializePMData(pmVertices, pmEdges);

    // Simplify the mesh until the target vertex count is reached
    while (pmVertices.size() > targetVertexCount) {
        if(pmVertices.size() % 500000 == 0)
            std::cout << "Cycling... Size:" << pmVertices.size() << std::endl;
        // Find the edge with the lowest error
        PMEdge* minErrorEdge = *std::min_element(pmEdges.begin(), pmEdges.end(),
            [](const PMEdge* a, const PMEdge* b) { return a->error < b->error; });

        // Update the position of the first vertex of the edge
        glm::vec4 optimalPosition = findOptimalPosition(minErrorEdge->v1->quadric + minErrorEdge->v2->quadric,
            minErrorEdge->v1, minErrorEdge->v2);
        minErrorEdge->v1->position = glm::vec3(optimalPosition);


        // Update the quadric of the first vertex
        minErrorEdge->v1->quadric += minErrorEdge->v2->quadric;

        // Remove the second vertex from the mesh
        pmVertices.erase(std::remove(pmVertices.begin(), pmVertices.end(), minErrorEdge->v2), pmVertices.end());

        // Update the edges connected to the removed vertex
        std::vector<PMEdge*> edgesToUpdate;
        for (PMEdge* edge : minErrorEdge->v2->edges) {
            if (edge != minErrorEdge) {
                edgesToUpdate.push_back(edge);
            }
        }

        for (PMEdge* edge : edgesToUpdate) {
            edge->v1 == minErrorEdge->v2 ? edge->v1 = minErrorEdge->v1 : edge->v2 = minErrorEdge->v1;
            edge->error = calculateEdgeError(edge);
            minErrorEdge->v1->edges.push_back(edge);
        }


        // Remove the collapsed edge from the mesh
        pmEdges.erase(std::remove(pmEdges.begin(), pmEdges.end(), minErrorEdge), pmEdges.end());
    }
}

void Chunk::createFromPMData(const std::vector<PMVertex*>& vertices, const std::vector<PMEdge*>& edges) {
    for (const auto& vertex : vertices) {
        addVertex(vertex->position);
    }

    for (const auto& edge : edges) {
        Vertex* start = verticies[edge->v1->id];
        Vertex* end = verticies[edge->v2->id];
        addEdge(start, end);
    }

    // Note: You might need to update the face creation logic depending on how you store the face data in your original Chunk
    for (const auto& face : faces) {
        addFace(face->edge->start->id, face->edge->end->id, face->edge->next->end->id);
    }
}


glm::vec3 calculateNewPosition(PMEdge* edge) {
    // Implement the QEM calculation for the new position
    // This will depend on your specific implementation of QEM
    // For now, we'll return the midpoint of the edge as a simple approximation
    return (edge->v1->position + edge->v2->position) * 0.5f;
}

void replaceVertexInEdge(PMEdge* edge, PMVertex* oldVertex, PMVertex* newVertex) {
    if (edge->v1 == oldVertex) {
        edge->v1 = newVertex;
    }
    else if (edge->v2 == oldVertex) {
        edge->v2 = newVertex;
    }

    // Remove the edge from the old vertex's list of edges
    auto edgeIter = std::find(oldVertex->edges.begin(), oldVertex->edges.end(), edge);
    oldVertex->edges.erase(edgeIter);

    // Add the edge to the new vertex's list of edges
    newVertex->edges.push_back(edge);
}

void collapseEdge(PMEdge* edge, std::vector<PMVertex*>& vertices, std::vector<PMEdge*>& edges) {
    // Calculate the new position of the collapsed vertex using QEM
    glm::vec3 newPosition = calculateNewPosition(edge);

    // Remove the edge from the list of edges
    auto edgeIter = std::find(edges.begin(), edges.end(), edge);
    edges.erase(edgeIter);

    // Replace the two vertices with the new collapsed vertex
    PMVertex* collapsedVertex = new PMVertex();
    collapsedVertex->position = newPosition;
    vertices.push_back(collapsedVertex);

    // Update the edges connected to the collapsed vertices
    for (auto& e : edge->v1->edges) {
        if (e != edge) {
            replaceVertexInEdge(e, edge->v1, collapsedVertex);
        }
    }
    for (auto& e : edge->v2->edges) {
        if (e != edge) {
            replaceVertexInEdge(e, edge->v2, collapsedVertex);
        }
    }

    // Remove the original vertices from the list of vertices
    vertices.erase(std::remove(vertices.begin(), vertices.end(), edge->v1), vertices.end());
    vertices.erase(std::remove(vertices.begin(), vertices.end(), edge->v2), vertices.end());
}

void Chunk::initializePMData(std::vector<PMVertex*>& pmVertices, std::vector<PMEdge*>& pmEdges) {
    //std::cout << "First for" << std::endl;
    for (const auto& vertex : verticies) {
        PMVertex* pmVertex = new PMVertex();
        pmVertex->id = vertex->id;
        pmVertex->position = vertex->point;
        pmVertices.push_back(pmVertex);
    }

    //std::cout << "Second for" << std::endl;
    for (const auto& face : faces) {
        glm::vec3 normal = face->getNormal();
        float distance = -glm::dot(normal, face->edge->start->point);
        glm::vec4 plane(normal, distance);

        for (Edge* e = face->edge; e->next != face->edge; e = e->next) {
            unsigned int vertexId = e->start->id;
            pmVertices[vertexId]->quadric += glm::outerProduct(plane, plane);
        }
    }

    //std::cout << "Third for" << std::endl;
    for (const auto& edge : this->edges) {
        PMEdge* pmEdge = new PMEdge();
        pmEdge->v1 = pmVertices[edge->start->id];
        pmEdge->v2 = pmVertices[edge->end->id];
        //std::cout << "Start calculateEdgeError" << std::endl;
        pmEdge->error = calculateEdgeError(pmEdge);
        pmEdges.push_back(pmEdge);

        pmEdge->v1->edges.push_back(pmEdge);
        pmEdge->v2->edges.push_back(pmEdge);
    }
}

float Chunk::calculateEdgeError(PMEdge* edge) {
    glm::mat4 sumQuadrics = edge->v1->quadric + edge->v2->quadric;

    // Find the optimal vertex position that minimizes the QEM error
    //std::cout << "Start findOptimalPosition" << std::endl;
    glm::vec4 optimalPosition = findOptimalPosition(sumQuadrics, edge->v1, edge->v2);

    // Calculate the QEM error for the edge
    float error = glm::dot(optimalPosition * sumQuadrics, optimalPosition);

    return error;
}

glm::vec4 Chunk::findOptimalPosition(const glm::mat4& sumQuadrics, const PMVertex* v1, const PMVertex* v2) {
    glm::mat4 A = sumQuadrics;
    A[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    glm::vec4 b = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    // Solve the linear system Ax = b
    glm::vec4 x;
    if (glm::determinant(A) != 0.0f) {
        x = glm::inverse(A) * b;
    }
    else {
        // If the matrix is singular, use the midpoint of the edge as a fallback
        x = glm::vec4(v1->position + v2->position, 1.0f) * 0.5f;
    }

    return x;
}







