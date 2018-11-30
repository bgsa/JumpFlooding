#include "Painter.h"

bool isPainting = false;
Vec2f mousePosition = Vec2f{ -1.0f, -1.0f };

void Painter::init()
{
	string vertexShaderSource = "#version 300 es \n"
		"uniform mat4 projectionView; \n"
		"uniform mat4 model; \n"
		"uniform vec4 Color; \n"

		"in  vec2 Position; \n"
		"in  vec2 Texture;  \n"
		
		"void main() \n"
		"{													\n"
		"	gl_Position = projectionView * model * vec4(Position, 0.0, 1.0);	\n"
		"}																		\n";
	
	string fragmentShaderSource = "#version 300 es \n"
		"precision highp float;	\n"
		"precision highp int;	\n"
		
		"uniform sampler2D inputColorTexture; \n"
		"uniform vec2 mousePosition; \n"
		"uniform vec2 painterScreenSize; \n"

		"layout(location = 0) out vec4 outputColor; \n"
		
		"void main() \n"
		"{ \n"
			"vec2 textureCoordinate = gl_FragCoord.xy / painterScreenSize.xy;								\n"
			"outputColor    = texture2D(inputColorTexture, textureCoordinate);							\n"

			"bool isDrawing = mousePosition != vec2(-1.0);													\n"

			"if (isDrawing) {																				\n"
				"float ray = distance( mousePosition , gl_FragCoord.xy );									\n"

				"if (ray < 7.0)																				\n"
					"outputColor				   = vec4( 0.0, 0.0, 1.0, 0.0 );							\n"
			"} \n"
		"} \n";
		
	programShader = Shader::loadShaderProgram(vertexShaderSource.c_str(), fragmentShaderSource.c_str());

	initVBO();
}

void Painter::copyFromBufferToTexture(GLuint framebuffer, GLenum buffer, GLuint texture, GLenum textureUnit, size_t width, size_t height, std::string filename)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);	
	glReadBuffer(buffer);

	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, texture);	
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, width, height, 0);
	
	/*
	unsigned char* pixels = new unsigned char[4 * width*height];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	Framebuffer::saveImage(filename + ".png", pixels, width, height);
	delete[] pixels;
	*/
}

GLuint Painter::generateFramebuffer(size_t width, size_t height)
{
	GLuint outputColorTexure;
	glGenTextures(1, &outputColorTexure);
	glBindTexture(GL_TEXTURE_2D, outputColorTexure);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

	GLuint outputSeedX;
	glGenTextures(1, &outputSeedX);
	glBindTexture(GL_TEXTURE_2D, outputSeedX);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

	GLuint outputSeedY;
	glGenTextures(1, &outputSeedY);
	glBindTexture(GL_TEXTURE_2D, outputSeedY);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

	GLenum framebufferAttachments[3] = { GL_COLOR_ATTACHMENT0 , GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

	GLuint customFramebuffer;
	glGenFramebuffers(1, &customFramebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, customFramebuffer);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, customFramebuffer);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	glFramebufferParameteri(GL_DRAW_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_WIDTH, width);
	glFramebufferParameteri(GL_DRAW_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_HEIGHT, height);

	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outputColorTexure, 0);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, outputSeedX, 0);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, outputSeedY, 0);

	glDrawBuffers(3, framebufferAttachments);

	GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		printf("There is a problem with the FBO!");

	return customFramebuffer;
}

void Painter::setupInputColor(float* pixels, size_t width, size_t height)
{
	glGenTextures(1, &inputColorTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, inputColorTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, //target
		0, //level
		GL_RGBA, //GL_RGB, //internalFormat
		width, //width,
		height, //height,
		0, //border
		GL_RGBA, //format GL_RGB  or GL_RGBA (format of data read from textel file) (GL_RGB for *.bmp files and GL_RGBA for *.rgb files) GL_ALPHA GL_RGB GL_RGBA GL_LUMINANCE GL_LUMINANCE_ALPHA
		GL_FLOAT, //type , GL_FLOAT
		pixels); //const void *pixels
}

void Painter::updateTexture(float* pixels, size_t width, size_t height, GLuint textureId)
{
	glBindTexture(GL_TEXTURE_2D, textureId);

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_FLOAT, pixels);

	glTexImage2D(GL_TEXTURE_2D, //target
		0, //level
		GL_RGBA, //GL_RGB, //internalFormat
		width, //width,
		height, //height,
		0, //border
		GL_RGBA, //format GL_RGB  or GL_RGBA (format of data read from textel file) (GL_RGB for *.bmp files and GL_RGBA for *.rgb files) GL_ALPHA GL_RGB GL_RGBA GL_LUMINANCE GL_LUMINANCE_ALPHA
		GL_FLOAT, //type , GL_FLOAT
		pixels); //const void *pixels
}

void Painter::setUpPositionAttribute()
{
	glVertexAttribPointer(positionAttribute,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		0); //Specify that our coordinate data is going into attribute index 0(shaderAttribute), and contains three floats per vertex

	glEnableVertexAttribArray(positionAttribute); //habilita atributo de coordenadas
}

void Painter::initVBO()
{	
	glGenBuffers(1, &vertexBufferObject);  //aloca o buffr
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject); //associa o bufffer ao ponteiro
	glBufferData(GL_ARRAY_BUFFER, sizeof(PainterShaderAttributes), &panelAttributes, GL_STATIC_DRAW);  //insere os dados no buffer para usar glDraw*
	
	projectionViewLocation = glGetUniformLocation(programShader, "projectionView");
	modelViewLocation = glGetUniformLocation(programShader, "model");
	colorLocation = glGetUniformLocation(programShader, "Color");
	mousePositionLocation = glGetUniformLocation(programShader, "mousePosition");
	painterScreenSizeLocation = glGetUniformLocation(programShader, "painterScreenSize");
		
	positionAttribute = glGetAttribLocation(programShader, "Position");

	inputColorTextureLocation = glGetUniformLocation(programShader, "inputColorTexture");

	setUpPositionAttribute();
}

void Painter::render(Mat4f projectionViewMatrix)
{
	glUseProgram(programShader);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	
	glUniform1i(inputColorTextureLocation, 0);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, inputColorTexture);
		
	glUniformMatrix4fv(projectionViewLocation, 1, GL_FALSE, projectionViewMatrix);
	glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, modelView);
	glUniform4f(colorLocation, color->Red, color->Green, color->Blue, color->Alpha);
	glUniform2f(mousePositionLocation, mousePosition.x(), mousePosition.y());
	glUniform2f(painterScreenSizeLocation, getWidth(), getHeight());	
		
	setUpPositionAttribute();
	
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	copyFromBufferToTexture(0, GL_BACK, inputColorTexture, GL_TEXTURE0, getWidth(), getHeight(), "background");

	Log::glErrors(__FILE__, __LINE__);
}

void Painter::onMouseDown(MouseEvent e) 
{
	isPainting = true;
	mousePosition = e.currentPosition.toVec2();
}

void Painter::onMouseUp(MouseEvent e) 
{
	isPainting = false;
	mousePosition = { -1.0f , -1.0f };
}

void Painter::onMouseMove(MouseEvent e) 
{
	if (isPainting)
		mousePosition = e.currentPosition.toVec2();
}

Painter::~Painter()
{
}