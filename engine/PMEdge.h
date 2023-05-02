#pragma once
#include "PMVertex.h"

// PMVertex.h
class PMVertex;

class PMEdge {
public:
    PMVertex* v1;
    PMVertex* v2;
    float error;
};