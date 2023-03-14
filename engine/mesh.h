#pragma once
#include "node.h"

class LIB_API Mesh : public Node
{
	private:
		unsigned int vaoGlobal;
		unsigned int vboVertex;
		unsigned int vboFace;
		std::vector<glm::vec3> verticies;
		std::vector<unsigned int> faces;

		void triangle(int v0, int v1, int v2);
	public:
		Mesh(char* name);
		virtual ~Mesh();

		void addVertex(glm::vec3 vertex);
		std::vector<glm::vec3> getVertecies();
		void addFace(unsigned int face[3]);

		void initVAO();

		virtual void render(glm::mat4 inverseCamera_M);
};

