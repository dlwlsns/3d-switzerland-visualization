#pragma once
#include "node.h"

class LIB_API Mesh : public Node
{
	private:
		std::vector<glm::vec3> vertices;
		std::vector<unsigned int> faces;

		// Store vertex arrays and buffers for multiple levels of detail
		std::vector<unsigned int> vaoLevels;
		std::vector<unsigned int> vboVertexLevels;
		std::vector<unsigned int> vboFaceLevels;
		std::vector<std::vector<glm::vec3>> verticesLevels; // Stores vertices data for each LOD level
		std::vector<std::vector<unsigned int>> facesLevels; // Stores faces data for each LOD level
		std::vector<float> lodDistances;

		std::vector<float> lodFactors = { 0.0f, 0.5f, 0.8f, 0.95f };

		void triangle(int v0, int v1, int v2);
	public:
		Mesh(char* name);
		virtual ~Mesh();

		void addVertex(glm::vec3 vertex);
		std::vector<glm::vec3> getVertecies();
		void addFace(unsigned int face[3]);
		std::vector<unsigned int> getFaces();

		void initVAO();

		virtual void render(glm::mat4 inverseCamera_M);

		void setLODDistances(const std::vector<float>& distances);
		void generateLods();
		void simplify_mesh(unsigned int targetLod);
		void perform_edge_collapse(int faceIndex);
		float compute_collapse_cost(int faceIndex);
};

