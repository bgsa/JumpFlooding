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
		GLfloat texture[8];
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
	GLuint inputSeedX;
	GLuint inputSeedY;

	GLuint inputColorTextureLocation;
	GLuint inputSeedXLocation;
	GLuint inputSeedYLocation;

	GLuint panelSizeLocation;
	GLuint stepSizeLocation;
	
	void initVBO();
	void setUpPositionAttribute();

	float stepSize = 0.0;

	GLuint generateFramebuffer(size_t width, size_t height);
	void copyFromBufferToTexture(GLuint framebuffer, GLenum buffer, GLuint texture, GLenum textureUnit, size_t width, size_t height, std::string filename);
		
public:
	   	
	void setupInputColor(float* pixels, size_t width, size_t height);
	void setupDistanceMap(size_t width, size_t height);

	void updateTexture(float* pixels, size_t width, size_t height, GLuint textureId);

	void makeVoronoi(Mat4f projectionViewMatrix, std::vector<Point2D*> points);
	
	void init();

	void render(Mat4f projectionViewMatrix);

	string toString() {
		return "Panel 2D";
	}

	~Panel();
};
