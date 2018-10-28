#include <algorithm>
#include "Renderer.h"
#include "GLConfig.h"
#include "RendererSettings.h"
#include "Timer.h"
#include "Camera.h"
#include "Point2D.h"
#include "Line2D.h"
#include "Circle2D.h"
#include "GraphicObject.h"
#include "StringHelper.h"
#include "Polygon2D.h"
#include "OpenML/Rectangle2D.h"
#include "Panel.h"
#include "Framebuffer.h"

bool isRunning = true;

Timer timer;
Camera camera;

Panel* panel = new Panel;
std::vector<Point2D*> points;

void Renderer::onKeyDown(int keyCode) 
{
	bool enterPressed = keyCode == 257 || keyCode == 335;
	bool sPressed = keyCode == 83;

	if (sPressed) 
	{
		Framebuffer::saveFrambebuffer("teste.bmp", GL_BACK);
		return;
	}

	if ( ! enterPressed )
		return;

	float width = (float) RendererSettings::getInstance()->getWidth();
	float height = (float) RendererSettings::getInstance()->getHeight();
	Mat4f projectionViewMatrix = camera.getHUDProjectionMatrix(width, height);

	//panel->makeVoronoiCPU(projectionViewMatrix, points);
	panel->makeVoronoi(projectionViewMatrix, points);

	//clear points
	for (size_t i = 0; i < points.size(); i++)
	{
		removeGraphicObject(points[i]);
		delete points[i];
	}
	points.clear();
}

void Renderer::onMouseDown(MouseEvent e)
{
	if (e.button != MouseButton::LEFT)
		return;
	
	OpenML::Randomizer<int> colorRandomizer(0, 255);
	int red = colorRandomizer.rand();
	int green = colorRandomizer.rand();
	int blue = colorRandomizer.rand();
	//float blue = 0.0f;

	Point2D* point = new Point2D;
	point->setPosition(e.currentPosition);
	//point->setPointSize(30.0f);
	point->setPointSize(16.0f);
	//point->setPointSize(1.0f);
	point->setColor({ red/255.0f, green/255.0f, blue / 255.0f, 1.0f });
	point->init();
	points.push_back(point);

	addGraphicObject(point);
};

void Renderer::onMouseMove(MouseEvent e)
{
}

void Renderer::onMouseUp(MouseEvent e)
{
	
};

void Renderer::start()
{	
	size_t width = RendererSettings::getInstance()->getWidth();
	size_t height = RendererSettings::getInstance()->getHeight();
	unsigned char* data = Framebuffer::emptyImage(width, height, {255, 255, 255, 255});

	panel->setColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	panel->setWidth((float) RendererSettings::getInstance()->getWidth());
	panel->setHeight((float) RendererSettings::getInstance()->getHeight());
	panel->setUpImage(data, width, height);
	//panel->setupDistanceMap(width, height);
	panel->init();

	addGraphicObject(panel);

	delete[] data;

#ifdef WINDOWS
	while (isRunning)
	{
		update();
		render();

		Log::glErrors(__FILE__, __LINE__);
	}
#endif
}

void Renderer::addGraphicObject(GraphicObject* graphicObject)
{
	PointerInputDeviceHandler* pointerHandler = dynamic_cast<PointerInputDeviceHandler*>(graphicObject);

	if (pointerHandler != nullptr)
		for (PointerInputDevice* pointerInputDevice : pointerInputDevices)
			pointerInputDevice->addHandler(pointerHandler);


	KeyboardInputDeviceHandler* keyboardHandler = dynamic_cast<KeyboardInputDeviceHandler*>(graphicObject);

	if (keyboardHandler != nullptr)
		for (KeyboardInputDevice* keyboardInputDevice : keyboardInputDevices)
			keyboardInputDevice->addHandler(keyboardHandler);


	TouchInputDeviceHandler* touchHandler = dynamic_cast<TouchInputDeviceHandler*>(graphicObject);

	if (touchHandler != nullptr)
		for (TouchInputDevice* touchInputDevice : touchInputDevices)
			touchInputDevice->addHandler(touchHandler);


	WindowInputDeviceHandler* windowHandler = dynamic_cast<WindowInputDeviceHandler*>(graphicObject);

	if (windowHandler != nullptr)
		for (WindowInputDevice* windowInputDevice : windowInputDevices)
			windowInputDevice->addHandler(windowHandler);

	graphicObjects.push_back(graphicObject);
}

void Renderer::removeGraphicObject(GraphicObject* graphicObject) 
{
	vector<GraphicObject*>::iterator item = std::find(graphicObjects.begin(), graphicObjects.end(), graphicObject);

	if (item != graphicObjects.end())
		graphicObjects.erase(item);
}

void Renderer::resize(int width, int height)
{
	glViewport(0, 0, width, height);

	RendererSettings::getInstance()->setWidth(width);
	RendererSettings::getInstance()->setHeight(height);
}

void Renderer::init(DisplayDevice* displayDevice)
{
	this->displayDevice = displayDevice;

	Log::info("OpenGL Vendor: " + GLConfig::getGLVendor());
	Log::info("OpenGL Version: " + GLConfig::getGLVersion());
	Log::info("OpenGLSL Version: " + GLConfig::getGLShadingLanguageVersion());
	Log::info("OpenGL Renderer: " + GLConfig::getGLRenderer());
	Log::info("OpenGL Extensions:");

	for (string extension : GLConfig::getGLExtensions())
		Log::info(extension);

	timer.start();

	//glEnable(GL_CULL_FACE); //elimina os vértices que estão sendo renderizados atrás de outros vértices. Ex.: modelo 3D
	glEnable(GL_DEPTH_TEST); //elimina os vértices que sobrepoem outros vértices quando estão no mesmo eixo Z.
	glEnable(GL_BLEND);									  //enable alpha color
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);    //enable alpha color

	Vec3f cameraPosition = { 0.0f, 5.0f, 10.0f };
	Vec3f cameraTarget = { 0.0f, 3.0f, 0.0f };
	camera.init(cameraPosition, cameraTarget);
}


void Renderer::updateInputDevices(long long elapsedTime)
{
	for (InputDevice* device : pointerInputDevices)
		device->update(elapsedTime);

	for (InputDevice* device : keyboardInputDevices)
		device->update(elapsedTime);

	for (InputDevice* device : windowInputDevices)
		device->update(elapsedTime);

	for (InputDevice* device : touchInputDevices)
		device->update(elapsedTime);
}

void Renderer::update()
{
	long long elapsedTime = timer.getElapsedTime();
	//cout << "FPS: " << timer.getFramesPerSecond() << END_OF_LINE;
	//cout << elapsedTime << END_OF_LINE;

	updateInputDevices(elapsedTime);

	//camera
	camera.update(elapsedTime);
	
	//update graphs objects
	for (GraphicObject* graph : graphicObjects)
		graph->update(elapsedTime);

	//sprite.update(elapsedTime);

	timer.update();
}

void Renderer::render3D(Mat4f projectionViewMatrix)
{
}

void Renderer::render2D(Mat4f projectionViewMatrix)
{
	for (GraphicObject* graph : graphicObjects) {
		GraphicObject2D* graph2D = dynamic_cast<GraphicObject2D*>(graph);

		if (graph2D != nullptr)
			graph2D->render(projectionViewMatrix);
	}
}

void Renderer::render()
{
	int width = RendererSettings::getInstance()->getWidth();
	int height = RendererSettings::getInstance()->getHeight();
	ColorRGBAf backgroundColor = RendererSettings::getInstance()->getBackgroudColor().normalizeColor();

	glClearColor(backgroundColor.Red, backgroundColor.Green, backgroundColor.Blue, backgroundColor.Alpha);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	float aspectRatio = RendererSettings::getInstance()->getAspectRatio();
	camera.setPerspective(90.0f, aspectRatio, 1.0f, 1000.0f);
	Mat4f projectionViewMatrix = camera.getProjectionViewMatrix().clone();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	render3D(projectionViewMatrix);

	projectionViewMatrix = camera.getHUDProjectionMatrix(float(width), float(height)).clone();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	render2D(projectionViewMatrix);
	
	displayDevice->swapBuffer();
}

void Renderer::exit()
{
	isRunning = false;
}

/* TOUCH INPUT HANDLE */
void Renderer::onTouchDown(TouchItem* touchItem) 
{
	MouseEvent e;
	e.currentPosition = OpenML::Point2Df(float(touchItem->currentPosition.x), float(touchItem->currentPosition.y));
	e.button = MouseButton::LEFT;
	onMouseDown(e);
}
void Renderer::onTouchUp(TouchItem* touchItem) 
{
	MouseEvent e;
	e.currentPosition = OpenML::Point2Df(float(touchItem->currentPosition.x), float(touchItem->currentPosition.y));
	e.button = MouseButton::LEFT;
	onMouseUp(e);
}
void Renderer::onTouchMove(TouchItem* touchItem) 
{
	MouseEvent e;
	e.currentPosition = OpenML::Point2Df(float(touchItem->currentPosition.x), float(touchItem->currentPosition.y));
	onMouseMove(e);
}

void Renderer::onTouchPointerIn(TouchEvent e) 
{

};
void Renderer::onTouchPointerOut(TouchEvent e) 
{

};


void Renderer::addInputDevice(InputDevice* inputDevice)
{
	PointerInputDevice* pointerDevice = dynamic_cast<PointerInputDevice*>(inputDevice);
	if (pointerDevice)
		pointerInputDevices.push_back(pointerDevice);

	KeyboardInputDevice* keyboardDevice = dynamic_cast<KeyboardInputDevice*>(inputDevice);
	if (keyboardDevice)
		keyboardInputDevices.push_back(keyboardDevice);

	WindowInputDevice* windowDevice = dynamic_cast<WindowInputDevice*>(inputDevice);
	if (windowDevice)
		windowInputDevices.push_back(windowDevice);

	TouchInputDevice* touchDevice = dynamic_cast<TouchInputDevice*>(inputDevice);
	if (touchDevice)
		touchInputDevices.push_back(touchDevice);
}

void Renderer::onClose()
{
	this->exit();
}

Renderer::~Renderer()
{
	for (InputDevice* device : pointerInputDevices)
		delete device;

	delete displayDevice;
}

void Renderer::onKeyUp(int keyCode)
{
	if (keyCode == 27) //ESC pressed
		this->exit();
}
