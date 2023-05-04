#include "edge.h"

unsigned int Edge::idCounter = 0;

Edge::Edge(Vertex* start, Vertex* end) {
	this->id = idCounter;

	this->start = start;
	this->end = end;
    this->start->edges.push_back(this);
    this->end->edges.push_back(this);

    this->error = calculateEdgeError();
    this->deleted = false;

    this->next = nullptr;
    this->prev = nullptr;

	idCounter++;
}

Edge::~Edge() {

}

glm::vec4 Edge::findOptimalPosition(const glm::mat4& sumQuadrics) {
    glm::vec3 dum = (this->start->point + this->end->point) / 2.0f;
    return glm::vec4(dum.x, dum.y, dum.z, 1.0f);

    // This is broken
    /*glm::mat4 A = sumQuadrics;
    A[3] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    glm::vec4 b = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    // Solve the linear system Ax = b
    glm::vec4 x;
    if (glm::determinant(A) != 0.0f) {
        x = glm::inverse(A) * b;
    }
    else {
        // If the matrix is singular, use the midpoint of the edge as a fallback
        x = glm::vec4(start->point + end->point, 1.0f) * 0.5f;
    }

    return x;*/
}

float Edge::calculateEdgeError() {
    glm::mat4 sumQuadrics = start->quadric + end->quadric;

    // Find the optimal vertex position that minimizes the QEM error
    //std::cout << "Start findOptimalPosition" << std::endl;
    glm::vec4 optimalPosition = findOptimalPosition(sumQuadrics);

    // Calculate the QEM error for the edge
    float error = glm::dot(optimalPosition * sumQuadrics, optimalPosition);

    return error;
}