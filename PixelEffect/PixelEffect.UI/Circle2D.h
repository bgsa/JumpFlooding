#pragma once

#include "GlobalHeader.h"
#include <OpenML/Point2D.h>
#include "Point2D.h"
#include "ColorRGBA.h"
#include <OpenML/Colision2DStatus.h>
#include <OpenML/Circle2D.h>
#include "GraphicObject2D.h"

class Circle2D : public GraphicObject2D
{
private:

	OpenML::Point2Df center = { 200.0f, 200.0f };
	float raySize = 100.0f;
	size_t pointsCount = 0;
	
	void initVBO();
	void setUpPositionAttribute();

public:
	Circle2D();
	Circle2D(OpenML::Point2Df center, float ray);

	void init();

	OpenML::Point2Df getCenter();
	void setCenter(OpenML::Point2Df position);
		
	float getRaySize();
	void setRaySize(float raySize);
	
	void render(Mat4f projectionViewMatrix);

	OpenML::Circle2Df toOpenMLCircle()
	{
		return OpenML::Circle2Df(center, raySize);
	}

	operator OpenML::Circle2Df()
	{
		return toOpenMLCircle();
	}

	virtual string toString() {
		return "Circle 2D";
	}
};
