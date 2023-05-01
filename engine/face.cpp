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