#include <cstdlib>

#include "Log.h"
#include "Shader.h"
#include "Circle2D.h"
#include "GLConfig.h"
#include "StringHelper.h"
#include <OpenML/Circle2D.h>

OpenML::Point2Df Circle2D::getCenter()
{
	return center;
}
void Circle2D::setCenter(OpenML::Point2Df position)
{
	center = position;
}

float Circle2D::getRaySize()
{
	return raySize;
}
void Circle2D::setRaySize(float raySize)
{
	this->raySize = raySize;
}

void Circle2D::init()
{
	const char* vertexShaderSource = "#version 300 es \n"
		"uniform mat4 modelView; \n"
		"uniform mat4 projectionView; \n"
		"uniform vec4 Color; \n"

		"in  vec2 Position; \n"
		"out vec4 fragmentColor; \n"

		"void main() \n"
		"{											\n"
		"	gl_Position = projectionView * modelView * vec4(Position, 0.0, 1.0); \n"
		"	fragmentColor = Color; \n"
		"}											\n";

	const char* fragmentShaderSource = "#version 300 es \n"
		"precision mediump float; \n"
		"in  vec4 fragmentColor; \n"
		"out vec4 FragColor; \n"

		"void main() \n"
		"{\n"
		"	FragColor = fragmentColor; \n"
		"}";
	
	programShader = Shader::loadShaderProgram(vertexShaderSource, fragmentShaderSource);

	initVBO();
}

void Circle2D::setUpPositionAttribute()
{
	glVertexAttribPointer(positionAttribute,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		0); //Specify that our coordinate data is going into attribute index 0(shaderAttribute), and contains three floats per vertex
	glEnableVertexAttribArray(positionAttribute); //habilita atributo de coordenadas
}

void Circle2D::initVBO()
{
	float* points = toOpenMLCircle().calculatePoints(pointsCount);

	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * pointsCount * 2, points, GL_STATIC_DRAW);

	modelViewLocation = glGetUniformLocation(programShader, "modelView");
	projectionViewLocation = glGetUniformLocation(programShader, "projectionView");
	colorLocation = glGetUniformLocation(programShader, "Color");
	positionAttribute = glGetAttribLocation(programShader, "Position");
	
	setUpPositionAttribute();
}

void Circle2D::render(Mat4f projectionViewMatrix)
{
	glUseProgram(programShader);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

	glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, modelView);
	glUniformMatrix4fv(projectionViewLocation, 1, GL_FALSE, projectionViewMatrix);
	glUniform4f(colorLocation, color->Red, color->Green, color->Blue, color->Alpha);

	setUpPositionAttribute();

	glDrawArrays(GL_TRIANGLE_FAN, 0, pointsCount);

	renderBorder(projectionViewMatrix, pointsCount);
}

Circle2D::Circle2D()
{
	;
}
Circle2D::Circle2D(OpenML::Point2Df center, float raySize)
{
	setCenter(center);
	setRaySize(raySize);
}
