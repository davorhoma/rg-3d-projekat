#pragma once
#include <vector>

class Shape
{
private:
	std::vector<float> vertices;
	unsigned int VAO = 0, VBO = 0;

public:
	Shape(const std::vector<float>& verts) : vertices(verts) {}

	void setupBuffers();
	void draw();
};

