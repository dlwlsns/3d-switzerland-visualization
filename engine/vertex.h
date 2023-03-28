#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Vertex
{
	public:
		unsigned int id;
		glm::vec3 point;

		static unsigned int idCounter;

		Vertex(glm::vec3 point);
		~Vertex();
};

