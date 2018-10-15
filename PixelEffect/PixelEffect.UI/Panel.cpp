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
		"uniform vec2 panelSize; \n"
		"uniform float stepSize; \n"

		"in vec4 fragmentColor; \n"

		"out vec4 FragColor; \n"

		"void main() \n"
		"{ \n"
			"vec2 textureCoordinate = vec2(gl_FragCoord.x / panelSize.x, gl_FragCoord.y / panelSize.y);         \n"

			"FragColor = texture(image, textureCoordinate); \n"

			"if (stepSize != 0.0) {								\n"
				
				"vec2 neighborCoordinateLeftUpper   = vec2( (gl_FragCoord.x - stepSize) / panelSize.x, (gl_FragCoord.y + stepSize) / panelSize.y);	\n"
				"vec2 neighborCoordinateLeftCenter  = vec2( (gl_FragCoord.x - stepSize) / panelSize.x,  gl_FragCoord.y / panelSize.y );				\n"
				"vec2 neighborCoordinateLeftLower   = vec2( (gl_FragCoord.x - stepSize) / panelSize.x, (gl_FragCoord.y - stepSize) / panelSize.y );	\n"
				"vec2 neighborCoordinateCenterLower = vec2(  gl_FragCoord.x / panelSize.x            , (gl_FragCoord.y - stepSize) / panelSize.y );	\n"
				"vec2 neighborCoordinateCenterUpper = vec2(  gl_FragCoord.x / panelSize.x            , (gl_FragCoord.y + stepSize) / panelSize.y );	\n"
				"vec2 neighborCoordinateRightLower  = vec2( (gl_FragCoord.x + stepSize) / panelSize.x, (gl_FragCoord.y - stepSize) / panelSize.y );	\n"	
				"vec2 neighborCoordinateRightCenter = vec2( (gl_FragCoord.x + stepSize) / panelSize.x,  gl_FragCoord.y / panelSize.y );				\n"
				"vec2 neighborCoordinateRightUpper  = vec2( (gl_FragCoord.x + stepSize) / panelSize.x, (gl_FragCoord.y + stepSize) / panelSize.y );	\n"

				"if (texture(image, neighborCoordinateLeftUpper) != vec4(1.0))							\n"
				"	FragColor = texture(image, neighborCoordinateLeftUpper);	\n"

				"if (texture(image, neighborCoordinateLeftCenter) != vec4(1.0))							\n"
				"	FragColor = texture(image, neighborCoordinateLeftCenter);	\n"

				"if (texture(image, neighborCoordinateLeftLower) != vec4(1.0))							\n"
				"	FragColor = texture(image, neighborCoordinateLeftLower);	\n"

				"if (texture(image, neighborCoordinateCenterLower) != vec4(1.0))							\n"
				"	FragColor = texture(image, neighborCoordinateCenterLower);	\n"

				"if (texture(image, neighborCoordinateCenterUpper) != vec4(1.0))							\n"
				"	FragColor = texture(image, neighborCoordinateCenterUpper);	\n"

				"if (texture(image, neighborCoordinateRightLower) != vec4(1.0))							\n"
				"	FragColor = texture(image, neighborCoordinateRightLower);	\n"

				"if (texture(image, neighborCoordinateRightCenter) != vec4(1.0))					\n"
				"	FragColor = texture(image, neighborCoordinateRightCenter);						\n"

				"if (texture(image, neighborCoordinateRightUpper) != vec4(1.0))						\n"
				"	FragColor = texture(image, neighborCoordinateRightUpper);						\n"

			"} \n"
		"} \n";

	programShader = Shader::loadShaderProgram(vertexShaderSource.c_str(), fragmentShaderSource.c_str());

	initVBO();
}

void Panel::makeVoronoi(Mat4f projectionViewMatrix, std::vector<Point2D*> points) 
{
	size_t width = RendererSettings::getInstance()->getWidth();
	size_t height = RendererSettings::getInstance()->getHeight();

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	for (Point2D* point : points)
		point->render(projectionViewMatrix);
	
	unsigned char* pixels = nullptr;

	glDisable(GL_PROGRAM_POINT_SIZE);
	stepSize = int(logf(float(width * height))) * 2;

	//Framebuffer::saveFrambebuffer("temp.bmp", GL_BACK);
	   
	for (size_t i = 0; i < stepSize; i++)
	{
		pixels = Framebuffer::getFramebuffer(GL_BACK);
		updateImage(pixels, width, height);
		delete[] pixels;
		render(projectionViewMatrix);

		stepSize--;
	}
}

void Panel::setUpImage(unsigned char* pixels, size_t width, size_t height)
{
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//caso utilize mipmap ...
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//set texture border color
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);

	glTexImage2D(GL_TEXTURE_2D, //target
		0, //level
		GL_RGB, //GL_RGB, //internalFormat
		width, //width,
		height, //height,
		0, //border
		GL_RGB, //format GL_RGB  or GL_RGBA (format of data read from textel file) (GL_RGB for *.bmp files and GL_RGBA for *.rgb files) GL_ALPHA GL_RGB GL_RGBA GL_LUMINANCE GL_LUMINANCE_ALPHA
		GL_UNSIGNED_BYTE, //type , GL_FLOAT
		pixels); //const void *pixels
}

void Panel::updateImage(unsigned char* pixels, size_t width, size_t height) 
{
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(GL_TEXTURE_2D, //target
		0, //level
		GL_RGB, //GL_RGB, //internalFormat
		width, //width,
		height, //height,
		0, //border
		GL_RGB, //format GL_RGB  or GL_RGBA (format of data read from textel file) (GL_RGB for *.bmp files and GL_RGBA for *.rgb files) GL_ALPHA GL_RGB GL_RGBA GL_LUMINANCE GL_LUMINANCE_ALPHA
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
	
	setUpPositionAttribute();
}

void Panel::render(Mat4f projectionViewMatrix)
{
	glUseProgram(programShader);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

	glBindTexture(GL_TEXTURE_2D, texture);
	
	glUniformMatrix4fv(projectionViewLocation, 1, GL_FALSE, projectionViewMatrix);
	glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, modelView);
	glUniform4f(colorLocation, color->Red, color->Green, color->Blue, color->Alpha);
	glUniform2f(panelSizeLocation, getWidth(), getHeight() );
	glUniform1f(stepSizeLocation, stepSize);
	
	setUpPositionAttribute();

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

Panel::~Panel()
{
}