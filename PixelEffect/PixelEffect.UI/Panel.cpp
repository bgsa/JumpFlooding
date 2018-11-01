#include "Panel.h"

Vec2f pack2(float value)
{
	int ivalue = int(value * 256.0f * 256.0f);
	int ix = ivalue % 256;
	int iy = ivalue / 256;
	return Vec2f(float(ix) / 255.0f, float(iy) / 255.0f);
}

float unpack2(Vec2f v)
{
	int ix = int(round(v[0] * 255.0f));
	int iy = int(round(v[1] * 255.0f));
	int ivalue = ix + iy * 256.0f;
	return float(ivalue) / 256.0f / 256.0f;
}

Vec4f EncodeFloat(float v)
{
	Vec4f kEncodeMul = Vec4f(1.0, 255.0, 65025.0, 160581375.0);
	float kEncodeBit = 1.0 / 255.0;
	Vec4f enc = kEncodeMul * v;
	enc = enc.fractional();
	enc = enc - (Vec4f(enc[1] * kEncodeBit, enc[2] * kEncodeBit, enc[3] * kEncodeBit, enc[3] * kEncodeBit));	

	return enc;
}

float DecodeFloat(Vec4f enc)
{
	Vec4f kDecodeDot = Vec4f(1.0f, 1.0f / 255.0f, 1.0f / 65025.0f, 1.0f / 160581375.0f);
	return enc.dot(kDecodeDot);
}

void Panel::init()
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
		"uniform sampler2D inputSeedX; \n"
		"uniform sampler2D inputSeedY; \n"

		"uniform vec2 panelSize; \n"
		"uniform float stepSize; \n"

		"layout(location = 0) out vec4 outputColor; \n"
		"layout(location = 1) out vec4 outputSeedX; \n"
		"layout(location = 2) out vec4 outputSeedY; \n"

		"vec4 EncodeFloat(float v) \n"
		"{ \n"
			"vec4 kEncodeMul = vec4(1.0, 255.0, 65025.0, 160581375.0); \n"
			"float kEncodeBit = 1.0 / 255.0; \n"
			"vec4 enc = kEncodeMul * v; \n"
			"enc = fract(enc); \n"
			"enc -= (enc.yzww * kEncodeBit); \n"
			"return enc; \n"
		"} \n"

		"float DecodeFloat(vec4 enc) \n"
		"{ \n"
			"vec4 kDecodeDot = vec4(1.0, 1.0 / 255.0, 1.0 / 65025.0, 1.0 / 160581375.0); \n"
			"return dot(enc, kDecodeDot); \n"
		"} \n"

		"void main() \n"
		"{ \n"
			"float nearestDistanceFromSeed = 99999.0;																\n"
			"vec2  pixelSize			   = 1.0 / panelSize.xy;													\n"
			"vec2  textureCoordinate       = gl_FragCoord.xy / panelSize.xy;										\n"
			
			"outputColor				   = vec4( 0.0 );															\n"
			"outputSeedX				   = vec4( 0.0 );															\n"
			"outputSeedY				   = vec4( 0.0 );															\n"
										
			"if (stepSize != 0.0) {																					\n"

				"for (int x = -1; x <= 1; ++x) {																	\n"
					"for (int y = -1; y <= 1; ++y) {																\n"					

						"vec2 neighborCoordinate =  textureCoordinate + (vec2(y,x) * stepSize * pixelSize);			\n"
						"vec4 neighborColor = texture(inputColorTexture, neighborCoordinate);						\n"

						"bool outOfTextureRange = neighborCoordinate.x < 0.0 || neighborCoordinate.x > 1.0 || neighborCoordinate.y < 0.0  || neighborCoordinate.y > 1.0; \n"
						"if ( outOfTextureRange )									\n"
							"continue;												\n"
		
						"if( neighborColor == vec4( 0.0 ) ) \n"
							"continue; \n"

						"vec4 neighborSeedCoordinateXAsColor = texture(inputSeedX, neighborCoordinate);										\n"
						"vec4 neighborSeedCoordinateYAsColor = texture(inputSeedY, neighborCoordinate);										\n"
						"vec2 neighborSeedCoordinate = vec2( DecodeFloat(neighborSeedCoordinateXAsColor) , DecodeFloat(neighborSeedCoordinateYAsColor) );	\n"
						
						"float distanceFromNeighborSeed = distance(neighborSeedCoordinate, textureCoordinate);			\n"
				
						"if (distanceFromNeighborSeed < nearestDistanceFromSeed) {										\n"
							"outputColor			 = neighborColor;													\n"
							"outputSeedX			 = neighborSeedCoordinateXAsColor;									\n"
							"outputSeedY			 = neighborSeedCoordinateYAsColor;									\n"
							"nearestDistanceFromSeed = distanceFromNeighborSeed;										\n"							
						"}\n"						

					"} \n"
				"} \n"

			"} else	{																									\n"				
				"bool isSeed = texture(inputColorTexture, textureCoordinate).w == 1.0;									\n"

				"if ( isSeed ) {																						\n"
					"outputColor = texture(inputColorTexture, textureCoordinate);										\n"

					"outputSeedX = EncodeFloat( textureCoordinate.x );													\n"
					"outputSeedX.w = 1.0;																				\n"

					"outputSeedY = EncodeFloat( textureCoordinate.y );													\n"
					"outputSeedY.w = 1.0;																				\n"

				"} else	{																								\n"
					"outputColor = vec4( 0.0 );																			\n"
					"outputSeedX = vec4( 0.0 );																			\n"
					"outputSeedY = vec4( 0.0 );																			\n"
				"}																										\n"
			"}																											\n"

		"} \n";
	
	programShader = Shader::loadShaderProgram(vertexShaderSource.c_str(), fragmentShaderSource.c_str());

	initVBO();
}

ColorRGBAf getPixelFromBuffer(GLuint framebuffer, GLenum buffer, GLuint texture, GLenum textureUnit, size_t width, size_t height, int row, int column)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);	
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadBuffer(buffer);	
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, texture);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, width, height, 0);
	
	float* pixels = new float[4 * width*height];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, pixels);

	ColorRGBAf color = {
		pixels[row * width * 4 + column * 4 + 0],
		pixels[row * width * 4 + column * 4 + 1],
		pixels[row * width * 4 + column * 4 + 2],
		pixels[row * width * 4 + column * 4 + 3]
	};

	delete[] pixels;
	
	return color;
}

void Panel::makeVoronoi(Mat4f projectionViewMatrix, std::vector<Point2D*> points) 
{
	/*
	points.clear();

	Point2D* p1 = new Point2D();
	p1->setPosition({ 100.0f, 100.0f });
	p1->setColor({ 1.0f, 0.0f, 0.0f, 1.0f });
	p1->setPointSize(1.0f);
	p1->init();
	points.push_back(p1);

	Point2D* p2 = new Point2D();
	p2->setPosition({ 400.0f, 400.0f });
	p2->setColor({ 0.0f, 1.0f, 0.0f, 1.0f });
	p2->setPointSize(1.0f);
	p2->init();
	points.push_back(p2);
	*/

	size_t width = RendererSettings::getInstance()->getWidth();
	size_t height = RendererSettings::getInstance()->getHeight();
	unsigned char* pixels = nullptr;
	int stepCount = (int)log2f(float(width));
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glDisable(GL_PROGRAM_POINT_SIZE);

	for (Point2D* point : points)
		point->render(projectionViewMatrix);
	
	copyFromBufferToTexture(0, GL_BACK, inputColorTexture, GL_TEXTURE0, width, height, "color-init");
		
	GLuint customFramebuffer = generateFramebuffer(width, height);
		
	stepSize = 0.0f;
	render(projectionViewMatrix);	

	/*
	ColorRGBAf pixelColor = getPixelFromBuffer(customFramebuffer, GL_COLOR_ATTACHMENT0, inputColorTexture, GL_TEXTURE0, width, height, 100, 100);
	ColorRGBAf seedX = getPixelFromBuffer(customFramebuffer, GL_COLOR_ATTACHMENT1, inputSeedX, GL_TEXTURE1, width, height, 100, 100);
	ColorRGBAf seedY = getPixelFromBuffer(customFramebuffer, GL_COLOR_ATTACHMENT2, inputSeedY, GL_TEXTURE2, width, height, 100, 100);

	float positionX = DecodeFloat(seedX.toVec4());
	float positionY = DecodeFloat(seedY.toVec4());
	*/
		
	copyFromBufferToTexture(customFramebuffer, GL_COLOR_ATTACHMENT0, inputColorTexture, GL_TEXTURE0, width, height, "color-0");
	copyFromBufferToTexture(customFramebuffer, GL_COLOR_ATTACHMENT1, inputSeedX, GL_TEXTURE1, width, height, "seedX-0");
	copyFromBufferToTexture(customFramebuffer, GL_COLOR_ATTACHMENT2, inputSeedY, GL_TEXTURE2, width, height, "seedY-0");
	
	int contador = 1;
	for (; stepCount >= 0; stepCount--)
	{	
		stepSize = powf(2, stepCount);
		render(projectionViewMatrix);
		
		copyFromBufferToTexture(customFramebuffer, GL_COLOR_ATTACHMENT0, inputColorTexture, GL_TEXTURE0, width, height, "color-" + std::to_string(contador) );
		copyFromBufferToTexture(customFramebuffer, GL_COLOR_ATTACHMENT1, inputSeedX, GL_TEXTURE1, width, height, "seedX-" + std::to_string(contador) );
		copyFromBufferToTexture(customFramebuffer, GL_COLOR_ATTACHMENT2, inputSeedY, GL_TEXTURE2, width, height, "seedY-" + std::to_string(contador) );
		
		contador++;
	}

	glDeleteFramebuffers(1, &customFramebuffer);
}

void Panel::copyFromBufferToTexture(GLuint framebuffer, GLenum buffer, GLuint texture, GLenum textureUnit, size_t width, size_t height, std::string filename)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);	
	glReadBuffer(buffer);

	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, texture);	
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, width, height, 0);
	
	/*
	unsigned char* pixels = new unsigned char[4 * width*height];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	Framebuffer::saveImage("input-" + filename + ".png", pixels, width, height);
	delete[] pixels;
	*/
}

GLuint Panel::generateFramebuffer(size_t width, size_t height)
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

void Panel::setupInputColor(float* pixels, size_t width, size_t height)
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

void Panel::setupDistanceMap(size_t width, size_t height)
{
	float* emptyTexture = Framebuffer::emptyImage(width, height, ColorRGBAf(0.0f, 0.0f, 0.0f, 0.0f));

	glGenTextures(1, &inputSeedX);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, inputSeedX);

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
		emptyTexture); //const void *pixels

	glGenTextures(1, &inputSeedY);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, inputSeedY);

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
		emptyTexture); //const void *pixels

	delete[] emptyTexture;
}

void Panel::updateTexture(float* pixels, size_t width, size_t height, GLuint textureId)
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

void Panel::setUpPositionAttribute()
{
	glVertexAttribPointer(positionAttribute,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		0); //Specify that our coordinate data is going into attribute index 0(shaderAttribute), and contains three floats per vertex

	glEnableVertexAttribArray(positionAttribute); //habilita atributo de coordenadas
}

void Panel::initVBO()
{	
	glGenBuffers(1, &vertexBufferObject);  //aloca o buffr
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject); //associa o bufffer ao ponteiro
	glBufferData(GL_ARRAY_BUFFER, sizeof(PanelShaderAttributes), &panelAttributes, GL_STATIC_DRAW);  //insere os dados no buffer para usar glDraw*
	
	projectionViewLocation = glGetUniformLocation(programShader, "projectionView");
	modelViewLocation = glGetUniformLocation(programShader, "model");	
	colorLocation = glGetUniformLocation(programShader, "Color");
	panelSizeLocation = glGetUniformLocation(programShader, "panelSize");
	stepSizeLocation = glGetUniformLocation(programShader, "stepSize");	
	
	positionAttribute = glGetAttribLocation(programShader, "Position");

	inputColorTextureLocation = glGetUniformLocation(programShader, "inputColorTexture");
	inputSeedXLocation = glGetUniformLocation(programShader, "inputSeedX");
	inputSeedYLocation = glGetUniformLocation(programShader, "inputSeedY");

	setUpPositionAttribute();
}

void Panel::render(Mat4f projectionViewMatrix)
{
	glUseProgram(programShader);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

	glUniform1i(inputColorTextureLocation, 0);
	glUniform1i(inputSeedXLocation, 1);
	glUniform1i(inputSeedYLocation, 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, inputColorTexture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, inputSeedX);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, inputSeedY);
		
	glUniformMatrix4fv(projectionViewLocation, 1, GL_FALSE, projectionViewMatrix);
	glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, modelView);
	glUniform4f(colorLocation, color->Red, color->Green, color->Blue, color->Alpha);
	glUniform2f(panelSizeLocation, getWidth(), getHeight() );
	glUniform1f(stepSizeLocation, stepSize);
	
	setUpPositionAttribute();

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	Log::glErrors(__FILE__, __LINE__);
}

Panel::~Panel()
{
}