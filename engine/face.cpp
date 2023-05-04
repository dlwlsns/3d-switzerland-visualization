#include "face.h"
#include "edge.h"
#include <GL/glew.h>

unsigned int Face::idCounter = 0;

Face::Face(Edge* edge) {
	this->id = idCounter;

	this->edge = edge;

	idCounter++;
}

Face::~Face() {
	//delete edge;
}

glm::vec3 Face::getNormal() const {
    glm::vec3 A = edge->start->point;
    glm::vec3 B = edge->end->point;
    glm::vec3 C = edge->next->end->point;

    glm::vec3 AB = B - A;
    glm::vec3 AC = C - A;

    glm::vec3 normal = glm::cross(AB, AC);
    normal = glm::normalize(normal);

    return normal;
}

void Face::replace(Vertex* oldVertex, Vertex* newVertex) {
    Edge* currentEdge = edge;
    do {
        if (currentEdge->start == oldVertex) {
            currentEdge->start = newVertex;
        }
        if (currentEdge->end == oldVertex) {
            currentEdge->end = newVertex;
        }
        currentEdge = currentEdge->next;
    } while (currentEdge != edge);
}
