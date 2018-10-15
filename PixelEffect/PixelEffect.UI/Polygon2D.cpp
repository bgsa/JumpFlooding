#include "Polygon2D.h"

void Polygon2D::init()
{
	string vertexShaderSource = "#version 300 es \n"
		"uniform mat4 projectionView; \n"
		"uniform mat4 model; \n"
		"uniform vec4 Color; \n"

		"in  vec2 Position; \n"	
		"out vec4 fragmentColor; \n"

		"void main() \n"
		"{																	 \n"
		"	gl_Position = projectionView * model * vec4(Position, 0.0, 1.0); \n"
		"	fragmentColor = Color;											 \n"
		"}																	 \n";

	string fragmentShaderSource = "#version 300 es \n"
		"precision mediump float; \n"

		"in  vec4 fragmentColor; \n"
		"in  vec2 fragmentTexture; \n"

		"out vec4 FragColor; \n"
		"uniform sampler2D tex; \n"

		"void main() \n"
		"{           \n"
		"		FragColor = fragmentColor; \n"
		"}";

	programShader = Shader::loadShaderProgram(vertexShaderSource.c_str(), fragmentShaderSource.c_str());

	initVBO();
}

void Polygon2D::setUpPositionAttribute()
{
	glVertexAttribPointer(positionAttribute,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		0);
	glEnableVertexAttribArray(positionAttribute);
}

float* Polygon2D::getVertexBuffer()
{
	float* buffer = new float[vertexes.size() * 2];
	size_t bufferIndex = 0;

	for (OpenML::Point2Df point : vertexes)
	{
		buffer[bufferIndex] = point.x;
		buffer[bufferIndex + 1] = point.y;

		bufferIndex += 2;
	}

	return buffer;
}

void Polygon2D::initVBO()
{
	float* vertexBuffer = getVertexBuffer();

	glGenBuffers(1, &vertexBufferObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, vertexes.size() * sizeof(OpenML::Point2Df), vertexBuffer, GL_STATIC_DRAW);

	projectionViewLocation = glGetUniformLocation(programShader, "projectionView");
	modelViewLocation = glGetUniformLocation(programShader, "model");
	colorLocation = glGetUniformLocation(programShader, "Color");

	positionAttribute = glGetAttribLocation(programShader, "Position");
	
	setUpPositionAttribute();
}

void Polygon2D::render(Mat4f projectionViewMatrix)
{
	glUseProgram(programShader);
	
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

	glUniformMatrix4fv(projectionViewLocation, 1, GL_FALSE, projectionViewMatrix);
	glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, modelView);
	glUniform4f(colorLocation, color->Red, color->Green, color->Blue, color->Alpha);

	setUpPositionAttribute();

	glDrawArrays(GL_TRIANGLE_FAN, 0, vertexes.size());
}

vector<OpenML::Line2Df> Polygon2D::getLines() 
{
	vector<OpenML::Line2Df> lines;

	for (size_t i = 1; i < vertexes.size(); i++)
	{
		OpenML::Line2Df line = OpenML::Line2Df(vertexes[i - 1], vertexes[i]);
		lines.push_back(line);
	}

	OpenML::Line2Df line = OpenML::Line2Df(vertexes[vertexes.size() - 1], vertexes[0]);
	lines.push_back(line);

	return lines;
}

Polygon2D::~Polygon2D()
{
}
