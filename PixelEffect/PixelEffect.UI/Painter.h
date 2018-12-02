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
#include "PointerInputDevice.h"

class Painter : public GraphicObject2D, public PointerInputDeviceHandler
{
private:	
	
	struct PainterShaderAttributes {
		GLfloat position[8];
		GLfloat texture[8];
	};

	PainterShaderAttributes panelAttributes = PainterShaderAttributes
	{
		{
			0.0f, 0.0f, //bottom-left
			0.0f, 1.0f, //top-left
			1.0f, 0.0f, //bottom-right
			1.0f, 1.0f, //top-right	
		}
	};
	
	GLint positionAttribute;
	
	GLuint programShaderPainter;
	GLuint programShaderVoronoi;
	GLuint programShaderMedialAxis;

	GLuint inputColorTexture;
	GLuint inputSeedX;
	GLuint inputSeedY;

	GLuint inputColorTextureLocation;
	GLuint inputSeedXLocation;
	GLuint inputSeedYLocation;

	GLint mousePositionLocation;
	GLint panelSizeLocation;
	GLuint stepSizeLocation;
		
	void initVBO();
	void setUpPositionAttribute();

	float stepSize = 0.0;

	GLuint generateFramebuffer(size_t width, size_t height);
	void copyFromBufferToTexture(GLuint framebuffer, GLenum buffer, GLuint texture, GLenum textureUnit, size_t width, size_t height, std::string filename);
		
public:
   	
	void setupInputColor(float* pixels, size_t width, size_t height);
	
	void makeVoronoi(Mat4f projectionViewMatrix);
		
	void init();

	void render(Mat4f projectionViewMatrix);

	virtual void onMouseDown(MouseEvent e);
	virtual void onMouseUp(MouseEvent e);
	virtual void onMouseMove(MouseEvent e);

	string toString() {
		return "Panel 2D";
	}

	~Painter();
};
