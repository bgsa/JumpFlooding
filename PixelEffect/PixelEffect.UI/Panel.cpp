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

void Panel::init()
{
	string vertexShaderSource = "#version 300 es \n"
		"uniform mat4 projectionView; \n"
		"uniform mat4 model; \n"
		"uniform vec4 Color; \n"

		"in  vec2 Position; \n"
		"in  vec2 Texture;  \n"
		
		"out vec4 fragmentColor; \n"
		"out vec2 textureCoordinate; \n"

		"void main() \n"
		"{													\n"
		"	gl_Position = projectionView * model * vec4(Position, 0.0, 1.0);	\n"
		"	textureCoordinate = Texture;										\n"
		"	fragmentColor = Color; \n"
		"}																		\n";


	string fragmentShaderSource = "#version 300 es \n"
		"precision highp float;	\n"
		"precision highp int;	\n"
		"in vec4 fragmentColor; \n"
		"in vec2 textureCoordinate; \n"
		
		"uniform sampler2D inputColorTexture; \n"
		"uniform sampler2D inputDistanceMap; \n"

		"uniform vec2 panelSize; \n"
		"uniform vec2 pixelSize; \n"
		"uniform float stepSize; \n"

		"layout(location = 0) out vec4 outputColor; \n"
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

		"void main() \n"
		"{ \n"
			"float nearestDistanceFromSeed = 99999.0;																\n"
			//"vec2 textureCoordinate        = gl_FragCoord.xy / panelSize.xy;										\n"						
			
			"outputColor				   = vec4( 0.0 );															\n"
			"outputDistanceMap			   = vec4( 0.0 );															\n"
						
			"if (stepSize != 0.0) {																					\n"

				"for (int x = -1; x <= 1; x++) {																	\n"
					"for (int y = -1; y <= 1; y++) {																\n"					

						"vec2 neighborCoordinate =  textureCoordinate + (vec2(x,y) * pixelSize * stepSize);			\n"
						"vec4 neighborColor = texture(inputColorTexture, neighborCoordinate);						\n"

						"bool outOfTextureRange = neighborCoordinate.x < 0.0 || neighborCoordinate.x > 1.0 || neighborCoordinate.y < 0.0  || neighborCoordinate.y > 1.0; \n"
						"if ( outOfTextureRange )									\n"
							"continue;												\n"
		
						"if( neighborColor == vec4( 0.0 ) ) \n"
							"continue; \n"

						"vec4 neighborSeedCoordinateAsColor = texture(inputDistanceMap, neighborCoordinate);										\n"
						"vec2 neighborSeedCoordinate = vec2( unpack2(neighborSeedCoordinateAsColor.xy) , unpack2(neighborSeedCoordinateAsColor.zw) );	\n"
				
						"float distanceFromNeighborSeed = distance(neighborSeedCoordinate, textureCoordinate);			\n"
				
						"if (distanceFromNeighborSeed < nearestDistanceFromSeed) {										\n"
							"outputColor			 = neighborColor;													\n"
							"outputDistanceMap		 = neighborSeedCoordinateAsColor;									\n"
							"nearestDistanceFromSeed = distanceFromNeighborSeed;										\n"							
						"}\n"						

					"} \n"
				"} \n"

			"} else	{																									\n"				
				"bool isSeed = texture(inputColorTexture, textureCoordinate).w == 1.0;									\n"

				"if ( isSeed ) {																						\n"
					"outputColor = texture(inputColorTexture, textureCoordinate);										\n"
					"outputDistanceMap = vec4( pack2(textureCoordinate.x) , pack2(textureCoordinate.y) );				\n"
				"} else	{																								\n"
					"outputColor = vec4( 0.0 );																			\n"
					"outputDistanceMap = vec4( 0.0 );																	\n"
				"}																										\n"
			"}																											\n"

		"} \n";
	
	programShader = Shader::loadShaderProgram(vertexShaderSource.c_str(), fragmentShaderSource.c_str());

	initVBO();
}

Vec4f texture(unsigned char * texture, int width, int height, int channels, Vec2f coordinate) 
{
	int index = ((coordinate.x() * width) - 1.0f) * width * channels + ((coordinate.y() * height) - 1.0f) * channels;

	Vec4f result = {
		float(texture[index + 0]),
		float(texture[index + 1]),
		float(texture[index + 2]),
		float(texture[index + 3])
	};

	return result;
}

void processaFragmento(
	int row,
	int column,
	Vec2f panelSize,
	unsigned char * inputColor, 
	unsigned char * inputDistance, 
	unsigned char * outputColor, 
	unsigned char * outputDistance,
	int stepSize)
{
	float nearestDistanceFromSeed = 99999.0f;
	Vec2f textureCoordinate        = Vec2f(row / panelSize.x(), column / panelSize.y());
	Vec4f nearestSeedColor         = texture(inputColor, panelSize[0], panelSize[1], 4, textureCoordinate);						
	Vec4f positionAsColor          = texture(inputDistance, panelSize[0], panelSize[1], 4, textureCoordinate);
	positionAsColor /= 255.0f;
	Vec2f nearestSeedCoordinate    = Vec2f( unpack2( Vec2f(positionAsColor.x(), positionAsColor.y()) ) , unpack2( Vec2f(positionAsColor.z(), positionAsColor.w()) ) );

	if ( nearestSeedCoordinate != Vec2f(0.0f, 0.0f) )
		nearestDistanceFromSeed = nearestSeedCoordinate.distance(textureCoordinate);

	for (int y = -1; y <= 1; ++y)
	{
		for (int x = -1; x <= 1; ++x) 
		{
		}
	}
}

void Panel::makeVoronoiCPU(Mat4f projectionViewMatrix, std::vector<Point2D*> points) 
{
	int width = 8;
	int height = 7;
	Vec2f panelSize = Vec2f( width, height );
	int channels = 4;

	Vec2f point1OnTexture = Vec2f(3.0f, 3.0f);
	Vec2f point2OnTexture = Vec2f(6.0f, 7.0f);

	Vec4f point1Color = Vec4f( 255.0f, 0.0f, 0.0f, 255.0f);
	Vec4f point2Color = Vec4f( 0.0f, 0.0f, 255.0f, 255.0f);
	Vec4f blackColor = Vec4f(0.0f, 0.0f, 0.0f, 255.0f);
	Vec4f blackTransparentColor = Vec4f(0.0f);

	unsigned char* inputColor = new unsigned char[channels * width * height];
	unsigned char* inputDistance = new unsigned char[channels * width * height];
		
	// INICIALIZA A TEXTURA WIDTH x HEIGHT and DISTANCE MAP
	int row = 0;
	int column = 0;
	for (size_t index = 0; index < channels * width * height; index += channels)
	{
		if (index % (width * channels) == 0) {
			row++;
			column = 1;
		}

		for (size_t i = 0; i < channels; i++)
		{
			inputColor[index + i] = 0;
			inputDistance[index + i] = blackTransparentColor[i];
		}
			
		if (Vec2f(row, column) == point1OnTexture) 
		{
			Vec4f positionAsColor = Vec4f(pack2(row / float(width)), pack2(column / float(height)));
			//positionAsColor *= 255.0f;

			for (size_t i = 0; i < channels; i++) 
			{
				inputColor[index + i] = point1Color[i];
				inputDistance[index + i] = positionAsColor[i];
			}	
		}

		if (Vec2f(row, column) == point2OnTexture) 
		{
			Vec4f positionAsColor = Vec4f(pack2(row / float(width)), pack2(column / float(height)));
			//positionAsColor *= 255.0f;
			
			for (size_t i = 0; i < channels; i++) 
			{
				inputColor[index + i] = point2Color[i];
				inputDistance[index + i] = positionAsColor[i];
			}
		}

		column++;
	}

	Framebuffer::saveImage("input-color-init.png", inputColor, width, height);
	Framebuffer::saveImage("input-distance-init.png", inputDistance, width, height);

	unsigned char* outputColor = new unsigned char[channels * width * height];
	unsigned char* outputDistance = new unsigned char[channels * width * height];

	//PROCESSA UM FRAGMENT SHADER
	row = 0;
	column = 0;
	for (size_t index = 0; index < channels * width * height; index += channels)
	{
		if (index % (width * channels) == 0) {
			row++;
			column = 1;
		}

		int stepSize = 0;
		processaFragmento(row, column, Vec2f(width, height) , inputColor, inputDistance, outputColor, outputDistance, stepSize);

		column++;
	}
}

void Panel::makeVoronoi(Mat4f projectionViewMatrix, std::vector<Point2D*> points) 
{
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
	
	copyFromBufferToTexture(customFramebuffer, GL_COLOR_ATTACHMENT0, inputColorTexture, GL_TEXTURE0, width, height, "color-0");
	copyFromBufferToTexture(customFramebuffer, GL_COLOR_ATTACHMENT1, inputDistanceMap, GL_TEXTURE1, width, height, "distance-map-0");
	
	int contador = 1;
	for (; stepCount > 0; stepCount--)
	{	
		stepSize = powf(2, stepCount);
		render(projectionViewMatrix);
		
		copyFromBufferToTexture(customFramebuffer, GL_COLOR_ATTACHMENT0, inputColorTexture, GL_TEXTURE0, width, height, "color-" + std::to_string(contador) );
		copyFromBufferToTexture(customFramebuffer, GL_COLOR_ATTACHMENT1, inputDistanceMap, GL_TEXTURE1, width, height, "distance-map-" + std::to_string(contador) );
		
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

	unsigned char* pixels = new unsigned char[4 * width*height];
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);;
	Framebuffer::saveImage("input-" + filename + ".png", pixels, width, height);
	delete[] pixels;
}

GLuint Panel::generateFramebuffer(size_t width, size_t height)
{
	GLuint outputColorTexure;
	glGenTextures(1, &outputColorTexure);
	glBindTexture(GL_TEXTURE_2D, outputColorTexure);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	GLuint outputDistanceMapTexture;
	glGenTextures(1, &outputDistanceMapTexture);
	glBindTexture(GL_TEXTURE_2D, outputDistanceMapTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	GLenum framebufferAttachments[2] = { GL_COLOR_ATTACHMENT0 , GL_COLOR_ATTACHMENT1 };

	GLuint customFramebuffer;
	glGenFramebuffers(1, &customFramebuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, customFramebuffer);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, customFramebuffer);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	glFramebufferParameteri(GL_DRAW_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_WIDTH, width);
	glFramebufferParameteri(GL_DRAW_FRAMEBUFFER, GL_FRAMEBUFFER_DEFAULT_HEIGHT, height);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outputColorTexure, 0);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, outputDistanceMapTexture, 0);

	glDrawBuffers(2, framebufferAttachments);

	GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
		printf("There is a problem with the FBO!");

	return customFramebuffer;
}

void Panel::setUpImage(unsigned char* pixels, size_t width, size_t height)
{
	glGenTextures(1, &inputColorTexture);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, inputColorTexture);

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

	unsigned char* emptyTexture = Framebuffer::emptyImage(width, height, ColorRGBAc( 0,0,0,0 ));

	glTexImage2D(GL_TEXTURE_2D, //target
		0, //level
		GL_RGBA, //GL_RGB, //internalFormat
		width, //width,
		height, //height,
		0, //border
		GL_RGBA, //format GL_RGB  or GL_RGBA (format of data read from textel file) (GL_RGB for *.bmp files and GL_RGBA for *.rgb files) GL_ALPHA GL_RGB GL_RGBA GL_LUMINANCE GL_LUMINANCE_ALPHA
		GL_UNSIGNED_BYTE, //type , GL_FLOAT
		emptyTexture); //const void *pixels

	delete[] emptyTexture;
}

void Panel::updateInputColorTexture(unsigned char* pixels, size_t width, size_t height, GLuint textureId)
{
	glBindTexture(GL_TEXTURE_2D, textureId);

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

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

void Panel::setUpTextureAttribute()
{
	glVertexAttribPointer(textureAttribute,
		2,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)sizeof(panelAttributes.position)); //Specify that our coordinate data is going into attribute index 0(shaderAttribute), and contains three floats per vertex
	glEnableVertexAttribArray(textureAttribute); //habilita atributo de coordenadas
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
	pixelSizeLocation = glGetUniformLocation(programShader, "pixelSize");
	
	positionAttribute = glGetAttribLocation(programShader, "Position");
	textureAttribute = glGetAttribLocation(programShader, "Texture");

	inputColorTextureLocation = glGetUniformLocation(programShader, "inputColorTexture");
	inputImageMapLocation = glGetUniformLocation(programShader, "inputDistanceMap");

	setUpPositionAttribute();
}

void Panel::render(Mat4f projectionViewMatrix)
{
	glUseProgram(programShader);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

	glUniform1i(inputColorTextureLocation, 0);
	glUniform1i(inputImageMapLocation, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, inputColorTexture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, inputDistanceMap);
		
	glUniformMatrix4fv(projectionViewLocation, 1, GL_FALSE, projectionViewMatrix);
	glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, modelView);
	glUniform4f(colorLocation, color->Red, color->Green, color->Blue, color->Alpha);
	glUniform2f(panelSizeLocation, getWidth(), getHeight() );
	glUniform1f(stepSizeLocation, stepSize);
	glUniform2f(pixelSizeLocation, 1.0f / getWidth(), 1.0f / getHeight());
	
	setUpPositionAttribute();
	setUpTextureAttribute();

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	Log::glErrors(__FILE__, __LINE__);
}

Panel::~Panel()
{
}