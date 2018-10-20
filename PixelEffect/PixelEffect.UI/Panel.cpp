#include "Panel.h"

void Panel::init()
{
	string vertexShaderSource = "#version 300 es \n"
		"uniform mat4 projectionView; \n"
		"uniform mat4 model; \n"
		"uniform vec4 Color; \n"

		"in  vec2 Position; \n"
		
		"out vec4 fragmentColor; \n"

		"void main() \n"
		"{													\n"
		"	gl_Position = projectionView * model * vec4(Position, 0.0, 1.0); \n"
		"	fragmentColor = Color; \n"
		"}													\n";


	string fragmentShaderSource = "#version 300 es \n"
		"precision mediump float; \n"

		"uniform sampler2D image; \n"
		"uniform sampler2D inputDistanceMap; \n"

		"uniform vec2 panelSize; \n"
		"uniform float stepSize; \n"

		"in vec4 fragmentColor; \n"

		"layout(location = 0) out vec4 FragColor; \n"
		"layout(location = 1) out vec4 outputDistanceMap; \n"
		
		"vec2 pack2(float value) {									\n"
		"	int ivalue = int(value * 256.0 * 256.0);				\n"
		"	int ix = ivalue % 256;									\n"
		"	int iy = ivalue / 256;									\n"
		"	return vec2(float(ix) / 255.0, float(iy) / 255.0);		\n"
		"}															\n"

		"float unpack2(vec2 v) {									\n"
		"	int ix = int(round(v.x*255.0));							\n"
		"	int iy = int(round(v.y*255.0));							\n"
		"	int ivalue = ix + iy * 256;								\n"
		"	return float(ivalue) / 256.0 / 256.0;					\n"
		"}															\n"

		"vec2 getCoordinatesFromTexture(vec2 textureCoordinate, sampler2D sampler) {				\n"
		"	vec4 positionAsColor = texture(sampler, textureCoordinate);								\n"
		"	vec2 coordinates = vec2( unpack2(positionAsColor.xy) , unpack2(positionAsColor.zw) );   \n"
		"	return coordinates;																		\n"
		"}																							\n"

		"void main() \n"
		"{ \n"
			"const vec4 whiteColor    = vec4(1.0);															\n"
			"vec2 textureCoordinate   = vec2(gl_FragCoord.x / panelSize.x, gl_FragCoord.y / panelSize.y);   \n"						
			"vec4 nearestSeedColor    = texture(image, textureCoordinate);									\n"
			"vec2 nearestSeedCoordinate = textureCoordinate;												\n"
			"float bestDistance = 99999.0;																	\n"
		
			"if (stepSize != 0.0) {																			\n"

			"	for (int y = -1; y <= 1; ++y) {																\n"
			"		for (int x = -1; x <= 1; ++x) {															\n"

						"vec2 neighborCoordinate =  textureCoordinate + vec2(x,y) * stepSize / panelSize.xy;				\n"
						"vec4 neighborColor = texture(image, neighborCoordinate);											\n"

						"vec4 positionAsColor = texture(inputDistanceMap, textureCoordinate);								\n"
						"vec2 neighborSeedCoordinate = vec2( unpack2(positionAsColor.xy) , unpack2(positionAsColor.zw) );	\n"

						"if ( neighborColor == whiteColor ) \n"
							"continue; \n"

						"float distanceFromNeighborSeed = distance(neighborSeedCoordinate, textureCoordinate);		\n"
		
						"if (distanceFromNeighborSeed < bestDistance) {												\n"
							"nearestSeedColor = texture(image, neighborSeedCoordinate);								\n"
							"nearestSeedCoordinate = neighborSeedCoordinate;										\n"
							"bestDistance = distanceFromNeighborSeed;												\n"							
						"}\n"
						
						
					"} \n"
				"} \n"

				"FragColor = nearestSeedColor; \n"
				"outputDistanceMap = vec4( pack2(nearestSeedCoordinate.x) , pack2(nearestSeedCoordinate.y) );	\n"

			"} else { \n"
				"FragColor = texture(image, textureCoordinate); \n"
				"outputDistanceMap = vec4( pack2(textureCoordinate.x) , pack2(textureCoordinate.y) );		\n"
			"} \n"
		"} \n";

	programShader = Shader::loadShaderProgram(vertexShaderSource.c_str(), fragmentShaderSource.c_str());

	initVBO();
}

void Panel::makeVoronoi(Mat4f projectionViewMatrix, std::vector<Point2D*> points) 
{
	size_t width = RendererSettings::getInstance()->getWidth();
	size_t height = RendererSettings::getInstance()->getHeight();
	unsigned char* pixels = nullptr;
	int stepCount = (int)log2f(float(width));
		
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glDisable(GL_PROGRAM_POINT_SIZE);

	for (Point2D* point : points)
		point->render(projectionViewMatrix);

	GLuint colorTexure;
	glGenTextures(1, &colorTexure);
	glBindTexture(GL_TEXTURE_2D, colorTexure);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	GLuint distanceMapTexture;
	glGenTextures(1, &distanceMapTexture);
	glBindTexture(GL_TEXTURE_2D, distanceMapTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);	

	GLenum framebufferAttachments[2] = { GL_COLOR_ATTACHMENT0 , GL_COLOR_ATTACHMENT1 };
	
	GLuint customFramebuffer;
	glGenFramebuffers(1, &customFramebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, customFramebuffer);

	glFramebufferParameteri(GL_DRAW_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_WIDTH, width);
	glFramebufferParameteri(GL_DRAW_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_HEIGHT, height);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexure, 0);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, distanceMapTexture, 0);

	glDrawBuffers(2, framebufferAttachments);
	
	GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		printf("There is a problem with the FBO!");


	stepSize = (float) int(width / 2.0f);

	glUseProgram(programShader);

	pixels = Framebuffer::getFramebuffer(GL_BACK);
	glActiveTexture(GL_TEXTURE0);
	updateInputImage(pixels, width, height, texture);
	delete[] pixels;

	unsigned char* blackTexture = Framebuffer::emptyImage(width, height, { 0,0,0,255 });
	glActiveTexture(GL_TEXTURE1);
	updateInputImage(blackTexture, width, height, inputDistanceMap);
	delete[] blackTexture;

	render(projectionViewMatrix);

	/*
	pixels = new unsigned char[4*width*height];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);
	Framebuffer::saveImage("texture.png", pixels, width, height);
	delete[] pixels;
	*/

	glBindFramebuffer(GL_READ_FRAMEBUFFER, customFramebuffer);
	Framebuffer::saveFrambebuffer("color.png", GL_COLOR_ATTACHMENT0);
	Framebuffer::saveFrambebuffer("distanceMap.png", GL_COLOR_ATTACHMENT1);

	/*
	//------------------------- render 2

	//pegar o colorTexture e atualizar a sampler com esse attachment
	glBindFramebuffer(GL_READ_FRAMEBUFFER, customFramebuffer);
	pixels = Framebuffer::getFramebuffer(GL_COLOR_ATTACHMENT0);
	glActiveTexture(GL_TEXTURE0);
	updateInputImage(pixels, width, height, texture);
	delete[] pixels;

	//pegar o distanceMap e atualizar a sampler de distancia com esse attachment
	pixels = Framebuffer::getFramebuffer(GL_COLOR_ATTACHMENT1);
	glActiveTexture(GL_TEXTURE1);
	updateInputImage(pixels, width, height, inputDistanceMap);
	delete[] pixels;
	
	stepSize /= 2.0f;
	
	render(projectionViewMatrix);

	// render 3

	//pegar o colorTexture e atualizar a sampler com esse attachment
	glBindFramebuffer(GL_READ_FRAMEBUFFER, customFramebuffer);
	pixels = Framebuffer::getFramebuffer(GL_COLOR_ATTACHMENT0);
	glActiveTexture(GL_TEXTURE0);
	updateInputImage(pixels, width, height, texture);
	delete[] pixels;

	//pegar o distanceMap e atualizar a sampler de distancia com esse attachment
	pixels = Framebuffer::getFramebuffer(GL_COLOR_ATTACHMENT1);
	glActiveTexture(GL_TEXTURE1);
	updateInputImage(pixels, width, height, inputDistanceMap);
	delete[] pixels;

	stepSize /= 2.0f;

	render(projectionViewMatrix);
	*/
	   
	/*
	for ( ; stepCount > 0 ; stepCount--)
	{		
	}
	*/

	glDeleteFramebuffers(1, &customFramebuffer);
}

void Panel::setUpImage(unsigned char* pixels, size_t width, size_t height)
{
	glGenTextures(1, &texture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, //target
		0, //level
		GL_RGBA, //GL_RGB, //internalFormat
		width, //width,
		height, //height,
		0, //border
		GL_RGBA, //format GL_RGB  or GL_RGBA (format of data read from textel file) (GL_RGB for *.bmp files and GL_RGBA for *.rgb files) GL_ALPHA GL_RGB GL_RGBA GL_LUMINANCE GL_LUMINANCE_ALPHA
		GL_UNSIGNED_BYTE, //type , GL_FLOAT
		pixels); //const void *pixels
}

void Panel::setupDistanceMap(size_t width, size_t height)
{
	glGenTextures(1, &inputDistanceMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, inputDistanceMap);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	unsigned char* blackTexture = Framebuffer::emptyImage(width, height, ColorRGBAc( 0,0,0,255 ));

	glTexImage2D(GL_TEXTURE_2D, //target
		0, //level
		GL_RGBA, //GL_RGB, //internalFormat
		width, //width,
		height, //height,
		0, //border
		GL_RGBA, //format GL_RGB  or GL_RGBA (format of data read from textel file) (GL_RGB for *.bmp files and GL_RGBA for *.rgb files) GL_ALPHA GL_RGB GL_RGBA GL_LUMINANCE GL_LUMINANCE_ALPHA
		GL_UNSIGNED_BYTE, //type , GL_FLOAT
		blackTexture); //const void *pixels

	delete[] blackTexture;
}

void Panel::updateInputImage(unsigned char* pixels, size_t width, size_t height, GLuint textureId)
{
	glBindTexture(GL_TEXTURE_2D, textureId);

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	/*
	glTexImage2D(GL_TEXTURE_2D, //target
		0, //level
		GL_RGBA, //GL_RGB, //internalFormat
		width, //width,
		height, //height,
		0, //border
		GL_RGBA, //format GL_RGB  or GL_RGBA (format of data read from textel file) (GL_RGB for *.bmp files and GL_RGBA for *.rgb files) GL_ALPHA GL_RGB GL_RGBA GL_LUMINANCE GL_LUMINANCE_ALPHA
		GL_UNSIGNED_BYTE, //type , GL_FLOAT
		pixels); //const void *pixels
		*/
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

	imageLocation = glGetUniformLocation(programShader, "image");
	inputImageMapLocation = glGetUniformLocation(programShader, "inputDistanceMap");

	setUpPositionAttribute();
}

void Panel::render(Mat4f projectionViewMatrix)
{
	glUseProgram(programShader);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

	glUniform1i(imageLocation, 0);
	glUniform1i(inputImageMapLocation, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, inputDistanceMap);
		
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