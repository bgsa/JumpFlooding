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
	int ivalue = ix + iy * 256;								
	return float(ivalue) / 256.0f / 256.0f;					
}	

void Panel::init()
{
	string vertexShaderSource = "#version 300 es \n"
		"uniform mat4 projectionView; \n"
		"uniform mat4 model; \n"
		"uniform vec4 Color; \n"

		"in  vec2 Position; \n"
		
		"void main() \n"
		"{													\n"
		"	gl_Position = projectionView * model * vec4(Position, 0.0, 1.0);	\n"
		"}																		\n";

	string fragmentShaderSource = "#version 300 es \n"
		"precision highp float;	\n"
		"precision highp int;	\n"

		"uniform sampler2D seedsColor; \n"
		"uniform sampler2D seedsPosition; \n"

		"uniform vec2 panelSize; \n"
		"uniform float stepSize; \n"

		"layout(location = 0) out vec4 outputColor; \n"
		
		"float unpack2(vec2 v) {									\n"
		"	int ix = int(round(v.x*255.0));							\n"
		"	int iy = int(round(v.y*255.0));							\n"
		"	int ivalue = ix + iy * 256;								\n"
		"	return float(ivalue) / 256.0 / 256.0;					\n"
		"}															\n"

		"void main() \n"
		"{ \n"
			"vec2 pixelSize = 1.0 / panelSize; \n"
			"vec2 textureCoordinate = gl_FragCoord.xy / panelSize.xy; \n"

			"if (stepSize != 0.0) {																					\n"
				"float nearestDistance = 9999.0;											\n"
				"float bestColor = vec4(0.0);												\n"

				"const float column = 0.0;													\n"
				"for (float row = 0.0; row <= panelSize.y * 1.5 ; row += 1.5) {					\n"					
					"vec2 seedCoordinate = vec2(row , column) * pixelSize;										\n"

					"vec4 color = texture(seedsColor, seedCoordinate);						\n"
					
					"if (color == vec4(0.0))				\n"
						"break;								\n"

					"vec4 positionAsColor = texture(seedsPosition, seedCoordinate);						\n"
					"vec2 position = vec2( unpack2(positionAsColor.xy) , unpack2(positionAsColor.zw) );		\n"
		
					"float distanceFromSeed = distance(position, textureCoordinate);			\n"

					"if (distanceFromSeed < nearestDistance) {										\n"
						"outputColor	 = color;													\n"
						"nearestDistance = distanceFromSeed;										\n"
					"}\n"

				"} \n"

			"} else	{																									\n"
				"bool isSeed = texture(seedsColor, textureCoordinate).w == 1.0;									\n"

				"if ( isSeed ) {																						\n"
					"outputColor = texture(seedsColor, textureCoordinate);										\n"			
				"} else	{																								\n"
					"outputColor = vec4( 0.0 );																			\n"					
				"}																										\n"
			"} \n"

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
	
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glDisable(GL_PROGRAM_POINT_SIZE);		
	glUseProgram(programShader);
	stepSize = 1.0f;

	unsigned char* seedsColor = new unsigned char[4 * width * height];
	unsigned char* seedsPosition = new unsigned char[4 * width * height];

	//Vec2f a = Vec2f(unpack2(Vec2f(0.0000f, 0.6314f)), unpack2(Vec2f(0.0000f, 0.4667f))) * 512;
	//Vec2f b = Vec2f(unpack2(Vec2f(0.0000f, 0.3451f)), unpack2(Vec2f(0.5020f, 0.1961f))) * 512;

	for (size_t i = 0; i < 4 * width * height; i++) 
	{
		seedsColor[i] = 0;
		seedsPosition[i] = 0;
	}

	int index = 0;
	for (Point2D* point : points) 
	{
		Vec2f position = point->getPosition().toVec2();
		Vec2f positionX = pack2(position.x() / width);
		Vec2f positionY = pack2(position.y() / height);
		
		ColorRGBAf* color = point->getColor();

		seedsColor[index + 0] = color->Red * 255;
		seedsColor[index + 1] = color->Green * 255;
		seedsColor[index + 2] = color->Blue * 255;
		seedsColor[index + 3] = color->Alpha * 255;

		seedsPosition[index + 0] = positionX[0] * 255;
		seedsPosition[index + 1] = positionX[1] * 255;
		seedsPosition[index + 2] = positionY[0] * 255;
		seedsPosition[index + 3] = positionY[1] * 255;
		
		index += 4;
	}
	
	//Framebuffer::saveImage("temp.png", seedsColor, width, height);
	
	updateTexture(seedsColor, width, height, inputColorTexture);
	updateTexture(seedsPosition, width, height, inputDistanceMap);
	
	delete[] seedsColor;
	delete[] seedsPosition;

	render(projectionViewMatrix);
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

	glGenTextures(1, &inputDistanceMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, inputDistanceMap);

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
		NULL); //const void *pixels
}

void Panel::updateTexture(unsigned char* pixels, size_t width, size_t height, GLuint textureId)
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

	inputColorTextureLocation = glGetUniformLocation(programShader, "seedsColor");
	inputImageMapLocation = glGetUniformLocation(programShader, "seedsPosition");

	setUpPositionAttribute();
}

void Panel::render(Mat4f projectionViewMatrix)
{
	Log::glErrors(__FILE__, __LINE__);

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

	setUpPositionAttribute();

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	Log::glErrors(__FILE__, __LINE__);
}

Panel::~Panel()
{
}