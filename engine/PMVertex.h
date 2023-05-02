#pragma once
#include "PMEdge.h"
#include <glm/glm.hpp>
#include <vector>


// PMEdge.h
class PMEdge; 

class PMVertex {
public:
    unsigned int id;
    glm::vec3 position;
    glm::mat4 quadric;
    std::vector<PMEdge*> edges;
};
