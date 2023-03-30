#pragma once

#include <vector>

#include "node.h"
#include "face.h"
#include "vertex.h"

class LIB_API Mesh : public Node
{
	private:
		unsigned int vaoGlobal;
		unsigned int vboVertex;
		unsigned int vboFace;
		std::vector<glm::vec3> verticies;
		std::vector<unsigned int> faces;
	public:
		Mesh(char* name);
		virtual ~Mesh();

		void addVerticies(std::vector<Vertex*> verticies);
		std::vector<glm::vec3> getVerticies();
		void addFaces(std::vector<Face*> faces);
		std::vector<unsigned int> getFaces();

		void initVAO();

		virtual void render(glm::mat4 inverseCamera_M);
};

