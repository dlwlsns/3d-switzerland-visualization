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

}

void Face::draw() {
	glVertex3f(edge->start->point.x, edge->start->point.y, edge->start->point.z);
	glVertex3f(edge->end->point.x, edge->end->point.y, edge->end->point.z);
	glVertex3f(edge->next->end->point.x, edge->next->end->point.y, edge->next->end->point.z);

	//glVertex3f(verticies[0]->point.x, verticies[0]->point.y, verticies[0]->point.z);
	//glVertex3f(verticies[1]->point.x, verticies[1]->point.y, verticies[1]->point.z);
	//glVertex3f(verticies[2]->point.x, verticies[2]->point.y, verticies[2]->point.z);
}