#pragma once

#include "GlobalHeader.h"
#include <vector>

class LineListStruct
{
private:
	vector<GLfloat> points;
	vector<GLfloat> colors;
	GLfloat defaultColor[4];

public:

	GLfloat* toArray();
	size_t getArraySize();
	void addPoint(float x, float y, float z);
	void addPoint(float x, float y, float z, float redColorFactor, float greenColorFactor, float blueColorFactor, float alphaFactor);
	size_t getPointCount();
	void setDefaultColor(GLfloat* color4f);
};

class LineList
{
private:
	Mat4f modelView = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};

	GLuint programShader;
	LineListStruct attributes;

	GLuint lineListVBO;

	GLint modelViewLocation;
	GLint projectionViewLocation;
	GLint positionAttribute;
	GLint colorAttribute;

	float lineWidth = 3.0f;

	void initVBO();
	void setUpPositionAttribute();
	void setUpColorAttribute();

public:
	~LineList();

	void init(LineListStruct attributes);

	void setLineWidth(float newLineWidth);

	void render(Mat4f projectionViewMatrix);
};
