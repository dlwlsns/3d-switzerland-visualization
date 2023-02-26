#pragma once
#include "node.h"
#include "material.h"

class LIB_API Mesh : public Node
{
	private:
		std::vector<glm::vec3> verticies;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> faces;
		Material* material;

		void triangle(int v0, int v1, int v2);
	public:
		Mesh(char* name);
		virtual ~Mesh();

		Material* getMaterial();
		void setMaterial(Material* material);

		void addVertex(glm::vec3 vertex);
		std::vector<glm::vec3> getVertecies();
		void addNormal(glm::vec3 normal);
		void addFace(unsigned int face[3]);

		virtual void render(glm::mat4 inverseCamera_M);
};

