#pragma once

#include <vector>
#include "vertex.h"
#include "edge.h"
#include "face.h"

#ifdef _WINDOWS 	
// Export API:
// Specifies i/o linkage (VC++ spec):
#ifdef CG_ENGINE_EXPORTS
#define LIB_API __declspec(dllexport)
#else
#define LIB_API __declspec(dllimport)
#endif      	
#else // Under Linux
#define LIB_API  // Dummy declaration
#endif

class LIB_API Chunk
{
private:
	
public:
	std::vector<Vertex*> verticies;
	std::vector<Edge*> edges;
	std::vector<Face*> faces;
	int x, z;

	Chunk(int x, int z);
	virtual ~Chunk();

	void setX(int x);
	void setZ(int z);

	void addVertex(glm::vec3 vertex);
	std::vector<Vertex*> getVertecies();
	void addEdge(Vertex* start, Vertex* end);
	void addFace(int v0, int v1, int v2);
	std::vector<Face*> getFaces();
	void empty();
	void simplify(unsigned int targetVertexCount);
};