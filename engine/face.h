#pragma once

#include <vector>
#include <string>
#include "edge.h"

class Face
{
	public:
		unsigned int id;
		Edge* edge;

		static unsigned int idCounter;

		Face(Edge* edge);
		~Face();

		glm::vec3 getNormal() const;
};

