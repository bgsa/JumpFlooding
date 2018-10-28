#pragma once

#include <cstdlib>
#include <vector>
#include "GlobalHeader.h"

#include <OpenML/Point2D.h>
#include "Log.h"
#include "Shader.h"
#include "Panel.h"
#include "ColorRGBA.h"
#include "GraphicObject2D.h"
#include "RendererSettings.h"
#include "Framebuffer.h"
#include "Point2D.h"

class Panel : public GraphicObject2D
{
private:	
	
	struct PanelShaderAttributes {
		GLfloat position[8];
	};

	PanelShaderAttributes panelAttributes = PanelShaderAttributes
	{
		{ //position  (space coordinates)
			0.0f, 0.0f, //bottom-left
			0.0f, 1.0f, //top-left
			1.0f, 0.0f, //bottom-right
			1.0f, 1.0f, //top-right	
		}
	};
	
	GLint positionAttribute;

	GLuint inputColorTexture;
	GLuint inputColorTextureLocation;

	GLuint inputDistanceMap;
	GLuint inputImageMapLocation;

	GLuint panelSizeLocation;
	GLuint stepSizeLocation;
	
	void initVBO();
	void setUpPositionAttribute();
	
	void copyFromBufferToTexture(GLuint framebuffer, GLenum buffer, GLuint texture, GLenum textureUnit, size_t width, size_t height, std::string filename);

	float stepSize = 0.0f;
		
public:
	   	
	void setUpImage(unsigned char* pixels, size_t width, size_t height);
	void updateTexture(unsigned char* pixels, size_t width, size_t height, GLuint textureId);
	
	void makeVoronoi(Mat4f projectionViewMatrix, std::vector<Point2D*> points);
	void makeVoronoiCPU(Mat4f projectionViewMatrix, std::vector<Point2D*> points);

	void setPositionAttributes(GLfloat* position)
	{
		for (size_t i = 0; i < 8; i++)
			panelAttributes.position[i] = position[i];
	}

	void init();

	void render(Mat4f projectionViewMatrix);

	string toString() {
		return "Panel 2D";
	}

	~Panel();
};
