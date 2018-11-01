#include "Framebuffer.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

unsigned char* Framebuffer::getFramebuffer(GLenum framebuffer)
{
	size_t width = RendererSettings::getInstance()->getWidth();
	size_t height = RendererSettings::getInstance()->getHeight();
	unsigned char* data = new unsigned char[4 * width * height];

	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	glReadBuffer(framebuffer);

	if (glGetError() == GL_INVALID_OPERATION)
		printf("Error setting FRAMEBUFFER");

	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

	if (glGetError() != GL_NO_ERROR)
		printf("Unknown error Reading Pixels");

	return data;
}

void Framebuffer::saveImage(std::string filename, unsigned char* pixels, size_t width, size_t height) 
{
	stbi_write_png(filename.c_str(), width, height, 4, pixels, 0);
}

void Framebuffer::saveFrambebuffer(std::string filename, GLenum framebuffer)
{
	size_t width = RendererSettings::getInstance()->getWidth();
	size_t height = RendererSettings::getInstance()->getHeight();

	unsigned char* data = getFramebuffer(framebuffer);

	Framebuffer::saveImage(filename, data, width, height);

	delete[] data;
}

unsigned char* Framebuffer::emptyImage(size_t width, size_t height, ColorRGBAc backgroundColor)
{
	const int channels = 4;

	unsigned char* data = new unsigned char[channels * width*height];

	for (size_t i = 0; i < channels * width*height; i += channels)
	{
		for (size_t j = 0; j < channels; j++)
			data[i + j] = backgroundColor[j];
	}

	return data;
}

float* Framebuffer::emptyImage(size_t width, size_t height, ColorRGBAf backgroundColor)
{
	const int channels = 4;

	float* data = new float[channels * width*height];

	for (size_t i = 0; i < channels * width*height; i += channels)
	{
		for (size_t j = 0; j < channels; j++)
			data[i + j] = backgroundColor[j];
	}

	return data;
}