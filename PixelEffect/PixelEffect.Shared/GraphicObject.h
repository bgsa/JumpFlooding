#pragma once

#include "Object.h"
#include  <OpenML/Mat4.h>

class GraphicObject : public Object
{
private:
	string tag;

protected:
	Mat4f modelView = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f,
	};

	GLuint programShader;
	GLuint vertexBufferObject;

	GLint modelViewLocation;
	GLint projectionViewLocation;
	GLint colorLocation;

	GLint positionAttribute;

public:

	void setTag(string tag)	{
		this->tag = tag;
	}
	string getTag() {
		return tag;
	}

	virtual void update(long long elapsedTime) { };
	virtual void render(Mat4f projectionViewMatrix) = 0;

	virtual ~GraphicObject() 
	{
		if (programShader != 0)
			glDeleteProgram(programShader);

		if (vertexBufferObject != 0)
			glDeleteBuffers(1, &vertexBufferObject);
	}

};