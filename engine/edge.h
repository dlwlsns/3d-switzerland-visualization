#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "vertex.h"

class Vertex;

class Edge
{
	public:
		unsigned int id;
		Vertex* start;
		Vertex* end;

		Edge* next;
		Edge* prev;
		Edge* twin;

		float error;
		bool deleted;

		static unsigned int idCounter;

		Edge(Vertex* start, Vertex* end);
		~Edge();

		glm::vec4 findOptimalPosition(const glm::mat4& sumQuadrics);
		float calculateEdgeError();
};

