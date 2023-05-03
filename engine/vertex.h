#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "edge.h"

class Edge;

class Vertex
{
	public:
		unsigned int id;
		glm::vec3 point;
		glm::mat4 quadric;
		bool deleted;
		std::vector<Edge*> edges;

		static unsigned int idCounter;

		Vertex(glm::vec3 point);
		~Vertex();
};

