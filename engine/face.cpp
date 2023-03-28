#include "face.h"
#include "edge.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

unsigned int Face::idCounter = 0;

Face::Face(Edge* edge) {
	this->id = idCounter;

	this->edge = edge;

	idCounter++;
}

Face::~Face() {
	//delete edge;
}

void Face::draw() {
	glVertex3f(edge->start->point.x, edge->start->point.y, edge->start->point.z);
	glVertex3f(edge->end->point.x, edge->end->point.y, edge->end->point.z);
	glVertex3f(edge->next->end->point.x, edge->next->end->point.y, edge->next->end->point.z);
}