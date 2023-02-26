#include <iostream>
#include <GL/freeglut.h>

#include "directionalLight.h"

DirectionalLight::DirectionalLight(char* name, float radius) : Light(name, radius) {
	this->setPositionW(0.0f);

	glLightfv(this->getLightId(), GL_POSITION, glm::value_ptr(this->getPosition()));
}

DirectionalLight::~DirectionalLight() {
	std::cout << "Deleted directional light" << std::endl;
}

void DirectionalLight::render(glm::mat4 inverseCamera) {
	Light::render(inverseCamera);
};