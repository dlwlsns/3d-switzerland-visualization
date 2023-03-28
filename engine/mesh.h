#pragma once
#include "node.h"
#include "face.h"
#include "edge.h"
#include "vertex.h"

class LIB_API Mesh : public Node
{
	private:
		unsigned int vaoGlobal;
		unsigned int vboVertex;
		unsigned int vboFace;
		std::vector<glm::vec3> v;
		std::vector<unsigned int> f;

		std::vector<Vertex *> verticies;
		std::vector<Edge*> edges;
		std::vector<Face *> faces;

		void triangle(int v0, int v1, int v2);
	public:
		Mesh(char* name);
		virtual ~Mesh();

		void addVertex(glm::vec3 vertex);
		std::vector<Vertex *> getVertecies();
		void addEdge(Vertex* start, Vertex* end);
		void addFace(int v0, int v1, int v2);

		void initVAO();

		virtual void render(glm::mat4 inverseCamera_M);
};

