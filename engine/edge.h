#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "vertex.h"

class Edge
{
	public:
		unsigned int id;
		Vertex* start;
		Vertex* end;

		Edge* next;
		Edge* prev;
		Edge* twin;

		static unsigned int idCounter;

		Edge(Vertex* start, Vertex* end);
		~Edge();
};

