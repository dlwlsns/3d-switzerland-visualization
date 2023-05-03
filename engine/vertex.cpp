#include "vertex.h"
#include <iostream>

unsigned int Vertex::idCounter = 0;

Vertex::Vertex(glm::vec3 point) {
	this->id = idCounter;
	this->point = point;

	this->deleted = false;

	idCounter++;
}

Vertex::~Vertex() {
	
}