#include <GL/freeglut.h>
#include <iostream>

#include "material.h"

Material::Material(char* name, glm::vec4 emission, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular, float shininess)
	: Object(name), emission(emission), ambient(ambient), diffuse(diffuse), specular(specular), shininess(shininess) {}

Material::~Material() {
	std::cout << "Deleted material" << std::endl;
}

glm::vec4 Material::getEmission() {
	return emission;
}

void Material::setEmission(glm::vec4 emission) {
	this->emission = emission;
}

glm::vec4 Material::getAmbient() {
	return ambient;
}

void Material::setAmbient(glm::vec4 ambient) {
	this->ambient = ambient;
}

glm::vec4 Material::getDiffuse() {
	return diffuse;
}

void Material::setDiffuse(glm::vec4 diffuse) {
	this->diffuse = diffuse;
}

glm::vec4 Material::getSpecular() {
	return specular;
}

void Material::setSpecular(glm::vec4 specular) {
	this->specular = specular;
}

float Material::getShininess() {
	return shininess;
}

void Material::setShininess(float shininess) {
	this->shininess = shininess;
}

void Material::render(glm::mat4 inverseCamera_M) {
	
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, glm::value_ptr(emission));
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glm::value_ptr(diffuse));

	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, glm::value_ptr(specular));
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, glm::value_ptr(ambient));
	glBindTexture(GL_TEXTURE_2D, 0);
}