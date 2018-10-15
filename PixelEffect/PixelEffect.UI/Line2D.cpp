#include <cstdlib>

#include "Log.h"
#include "Shader.h"
#include "Line2D.h"
#include "GLConfig.h"

bool Line2D::isOnTheLine(OpenML::Point2Df point)
{
	return toOpenMLLine2D().isOnTheLine(point);
}
bool Line2D::isOnTheLeft(OpenML::Point2Df point)
{
	return toOpenMLLine2D().isOnTheLeft(point);
}
bool Line2D::isOnTheRight(OpenML::Point2Df point)
{
	return toOpenMLLine2D().isOnTheRight(point);
}
OpenML::VerticalOrientation Line2D::getOrientation(OpenML::Point2Df point)
{
	return toOpenMLLine2D().getOrientation(point);
}

vector<OpenML::Point2Df> Line2D::findIntersection(Polygon2D* polygon) 
{
	vector<OpenML::Point2Df> intersectionPoints;

	vector<OpenML::Line2Df> polygonLines = polygon->getLines();
	OpenML::Line2Df line = toOpenMLLine2D();

	for (OpenML::Line2Df polygonLine : polygonLines)
	{
		OpenML::Point2Df* intersectionPoint = line.findIntersection(polygonLine);

		if (intersectionPoint != nullptr)
			intersectionPoints.push_back(*intersectionPoint);
	}

	return intersectionPoints;
}

OpenML::Point2Df* Line2D::findIntersection(Line2D* otherLine)
{
	OpenML::Point2Df* point = toOpenMLLine2D().findIntersection(otherLine->toOpenMLLine2D());
	return point;
}

Vec3f Line2D::getEquation() 
{
	SystemOfLinearEquations<float> system;
	Vec3f values = system.getLineEquation(getPoint1().toVec2(), getPoint2().toVec2());
	
	return values;
}

float Line2D::getDistance(Point2D point) 
{
	OpenML::Point2Df position = point.getPosition();
	return getDistance(position);
}

float Line2D::getDistance(OpenML::Point2Df position)
{
	Vec3f values = getEquation();

	float numerador = fabsf(values[0] * position.x + values[1] * position.y + values[2]);
	float denominador = sqrt(values[0] * values[0] + values[1] * values[1]);

	float distanceFromPointToLine = numerador / denominador;

	return distanceFromPointToLine;
}

OpenML::Colision2DStatus Line2D::hasIntersections(Circle2D* circle)
{
	float distanceCenterToLine  = ceil(getDistance(circle->getCenter()));
	float ray = ceil(circle->getRaySize());

	if (distanceCenterToLine > ray)
		return OpenML::Colision2DStatus::OUTSIDE;

	if (distanceCenterToLine < ray)
		return OpenML::Colision2DStatus::INSIDE;

	return OpenML::Colision2DStatus::INLINE;
}

OpenML::Point2Df Line2D::getPoint1()
{
	return OpenML::Point2Df{ attributes.point1[0] , attributes.point1[1] };
}
void Line2D::setPoint1(OpenML::Point2Df position)
{
	attributes.point1[0] = position.x;
	attributes.point1[1] = position.y;
}

OpenML::Point2Df Line2D::getPoint2()
{
	return OpenML::Point2Df{ attributes.point2[0] , attributes.point2[1] };
}
void Line2D::setPoint2(OpenML::Point2Df position)
{
	attributes.point2[0] = position.x;
	attributes.point2[1] = position.y;
}

void Line2D::setLineWidth(float newLineWidth)
{
	lineWidth = newLineWidth;
}

float Line2D::deltaX() 
{
	return fabsf(attributes.point1[0] - attributes.point2[0]);
}
float Line2D::deltaY() 
{
	return fabsf(attributes.point1[1] - attributes.point2[1]);
}

void Line2D::init()
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

void Line2D::setUpPositionAttribute()
{
	glVertexAttribPointer(positionAttribute,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		0); //Specify that our coordinate data is going into attribute index 0(shaderAttribute), and contains three floats per vertex
	glEnableVertexAttribArray(positionAttribute); //habilita atributo de coordenadas
}

void Line2D::initVBO()
{
	glGenBuffers(1, &vertexBufferObject);  //aloca o buffr
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject); //associa o bufffer ao ponteiro
	glBufferData(GL_ARRAY_BUFFER, sizeof(Line2DShaderAttributes), &attributes, GL_DYNAMIC_DRAW);

	modelViewLocation = glGetUniformLocation(programShader, "modelView");
	projectionViewLocation = glGetUniformLocation(programShader, "projectionView");
	colorLocation = glGetUniformLocation(programShader, "Color");

	positionAttribute = glGetAttribLocation(programShader, "Position");

	setUpPositionAttribute();
}

void Line2D::render(Mat4f projectionViewMatrix)
{
	glUseProgram(programShader);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Line2DShaderAttributes), &attributes, GL_DYNAMIC_DRAW);  //insere os dados no buffer para usar glDraw*

	glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, modelView);
	glUniformMatrix4fv(projectionViewLocation, 1, GL_FALSE, projectionViewMatrix);
	glUniform4f(colorLocation, color->Red, color->Green, color->Blue, color->Alpha);

	setUpPositionAttribute();

	float currentLineWidth = GLConfig::getGLfloat(GL_LINE_WIDTH);
	glLineWidth(lineWidth);

	glDrawArrays(GL_LINES, 0, 2);

	glLineWidth(currentLineWidth);
}

