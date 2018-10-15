#pragma once

#include "GlobalHeader.h"

struct GridSystemShaderAttributes {
	GLfloat point1[3];
	GLfloat point2[3];
	GLfloat pointColor1[4];
	GLfloat pointColor2[4];
};

class GridSystem
{
private:
	Mat4f modelView = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};

	GLuint programShader;
	GridSystemShaderAttributes attributes;

	GLuint lineVBO;

	GLint modelViewLocation;
	GLint projectionViewLocation;
	GLint positionAttribute;
	GLint colorAttribute;

	float lineWidth = 3.0f;

	void initVBO();
	void setUpPositionAttribute();
	void setUpColorAttribute();

public:
	~GridSystem();

	void init();

	void setLineWidth(float newLineWidth);

	void render(Mat4f projectionViewMatrix);
};
