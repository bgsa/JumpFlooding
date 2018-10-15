#pragma once

#include "GlobalHeader.h"
#include "OpenML/Point2D.h"
#include "OpenML/Line2D.h"
#include "GraphicObject2D.h"

#include "Log.h"
#include "Shader.h"
#include "ColorRGBA.h"

class Polygon2D : public GraphicObject2D
{
private:
	
	GLint positionAttribute;

	void initVBO();
	void setUpPositionAttribute();

	float* getVertexBuffer();
	
public:

	vector<OpenML::Point2Df> vertexes;

	vector<OpenML::Line2Df> getLines();
	
	void init();

	void render(Mat4f projectionViewMatrix);

	virtual string toString() {
		return "Polygon 2D";
	}

	~Polygon2D();
};

