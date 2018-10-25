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
		},
		{ //texture (u,v)
			0.0f, 0.0f,  // Top-left
			0.0f, 1.0f,  // Bottom-left
			1.0f, 0.0f,  // Top-right
			1.0f, 1.0f   // Bottom-right
		}
	};
	
	GLint positionAttribute;
	GLint textureAttribute;

	GLuint inputColorTexture;
	GLuint inputDistanceMap;

	GLuint inputColorTextureLocation;
	GLuint inputImageMapLocation;
	GLuint panelSizeLocation;
	GLuint stepSizeLocation;
	GLuint pixelSizeLocation;
	
	void initVBO();
	void setUpPositionAttribute();
	void setUpTextureAttribute();

	float stepSize = 0.0;

	GLuint generateFramebuffer(size_t width, size_t height);
	void copyFromBufferToTexture(GLuint framebuffer, GLenum buffer, GLuint texture, GLenum textureUnit, size_t width, size_t height, std::string filename);
		
public:
	   	
	void setUpImage(unsigned char* pixels, size_t width, size_t height);
	void updateInputColorTexture(unsigned char* pixels, size_t width, size_t height, GLuint textureId);

	void setupDistanceMap(size_t width, size_t height);

	void makeVoronoi(Mat4f projectionViewMatrix, std::vector<Point2D*> points);
	void makeVoronoiCPU(Mat4f projectionViewMatrix, std::vector<Point2D*> points);

	void setPositionAttributes(GLfloat* position)
	{
		for (size_t i = 0; i < 8; i++)
			panelAttributes.position[i] = position[i];
	}

	void setAttributes(GLfloat* position, GLfloat* textureCoord) 
	{
		for (size_t i = 0; i < 8; i++) 
		{
			panelAttributes.position[i] = position[i];
			panelAttributes.texture[i] = textureCoord[i];
		}
	}

	void init();

	void render(Mat4f projectionViewMatrix);

	string toString() {
		return "Panel 2D";
	}

	~Panel();
};
