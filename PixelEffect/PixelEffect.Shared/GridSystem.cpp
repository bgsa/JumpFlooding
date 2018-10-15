#include <cstdlib>

#include "Log.h"
#include "Shader.h"
#include "GridSystem.h"
#include "GLConfig.h"

GridSystemShaderAttributes gridSystemAttributes = GridSystemShaderAttributes{
	//position 
	{ 0.f, 0.f, -10.0f },
	{ 0.f, 0.f, 10.0f },
	//color 
	{ 1.0f, 0.0f, 0.0f, 1.0f },
	{ 1.0f, 0.0f, 0.0f, 1.0f }
};

void GridSystem::setLineWidth(float newLineWidth)
{
	lineWidth = newLineWidth;
}

void GridSystem::init()
{
	const char* vertexShaderSource = "#version 300 es \n"
		"uniform mat4 modelView; \n"
		"uniform mat4 projectionView; \n"

		"in  vec3 Position; \n"
		"in  vec4 Color; \n"

		"out vec4 fragmentColor; \n"

		"void main() \n"
		"{											\n"
		"	gl_Position = projectionView * modelView * vec4(Position, 1.0); \n"
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

	this->attributes = gridSystemAttributes;

	programShader = Shader::loadShaderProgram(vertexShaderSource, fragmentShaderSource);

	initVBO();
}

void GridSystem::setUpPositionAttribute()
{
	glVertexAttribPointer(positionAttribute,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		0); //Specify that our coordinate data is going into attribute index 0(shaderAttribute), and contains three floats per vertex
	glEnableVertexAttribArray(positionAttribute); //habilita atributo de coordenadas
}

void GridSystem::setUpColorAttribute()
{
	glVertexAttribPointer(colorAttribute,
		4,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)(sizeof(attributes.point1) + sizeof(attributes.point2)) // deslocamento do primeiro elemento
	);
	glEnableVertexAttribArray(colorAttribute); //habilita atributo de coordenadas
}

void GridSystem::initVBO()
{
	glGenBuffers(1, &lineVBO);  //aloca o buffr
	glBindBuffer(GL_ARRAY_BUFFER, lineVBO); //associa o bufffer ao ponteiro
	glBufferData(GL_ARRAY_BUFFER, sizeof(GridSystemShaderAttributes), &attributes, GL_STATIC_DRAW);  //insere os dados no buffer para usar glDraw*
																							   // Get the sampler location
	modelViewLocation = glGetUniformLocation(programShader, "modelView");
	projectionViewLocation = glGetUniformLocation(programShader, "projectionView");
	positionAttribute = glGetAttribLocation(programShader, "Position");
	colorAttribute = glGetAttribLocation(programShader, "Color");

	setUpPositionAttribute();
	setUpColorAttribute();
}

void GridSystem::render(Mat4f projectionViewMatrix)
{
	glUseProgram(programShader);

	glBindBuffer(GL_ARRAY_BUFFER, lineVBO);

	glUniformMatrix4fv(projectionViewLocation, 1, GL_FALSE, projectionViewMatrix);

	setUpPositionAttribute();
	setUpColorAttribute();

	float currentLineWidth = GLConfig::getGLfloat(GL_LINE_WIDTH);
	glLineWidth(lineWidth);



	float quantidade = 10.0f;
	Mat4f matrixRotate = Mat4f::createRotate((float)degreesToRadians(90), 0.0f, 1.0f, 0.0f);

	for (float i = -quantidade; i < quantidade + 1; i++)
	{
		//draw lines on Z axis
		Mat4f matrixTranslatedX = Mat4f::createTranslate(i, 0.0f, 0.0f);

		Mat4f result = modelView * matrixTranslatedX;

		glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, result);
		glDrawArrays(GL_LINES, 0, 2);

		//draw lines on X axis		
		Mat4f matrixTranslatedZ = Mat4f::createTranslate(0.0f, 0.0f, i);

		result = matrixRotate * modelView;

		Mat4f result2 = matrixTranslatedZ * result;

		glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, result2);
		glDrawArrays(GL_LINES, 0, 2);
	}


	glLineWidth(currentLineWidth);
}

GridSystem::~GridSystem()
{
	if (programShader != 0)
		glDeleteProgram(programShader);

	if (lineVBO != 0)
		glDeleteBuffers(1, &lineVBO);
}