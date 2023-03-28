#include "edge.h"

unsigned int Edge::idCounter = 0;

Edge::Edge(Vertex* start, Vertex* end) {
	this->id = idCounter;

	this->start = start;
	this->end = end;

	idCounter++;
}

Edge::~Edge() {

}