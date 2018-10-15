#include "Framebuffer.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

unsigned char* Framebuffer::getFramebuffer(GLenum framebuffer)
{
	size_t width = RendererSettings::getInstance()->getWidth();
	size_t height = RendererSettings::getInstance()->getHeight();
	unsigned char* data = new unsigned char[3 * width * height];

	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	glReadBuffer(framebuffer);

	if (glGetError() == GL_INVALID_OPERATION)
		printf("Error setting FRAMEBUFFER\n");

	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

	if (glGetError() != GL_NO_ERROR)
		printf("Unknown error Reading Pixels\n");

	return data;
}

void Framebuffer::saveFrambebuffer(std::string filename, GLenum framebuffer)
{
	size_t width = RendererSettings::getInstance()->getWidth();
	size_t height = RendererSettings::getInstance()->getHeight();

	unsigned char* data = getFramebuffer(framebuffer);

	stbi_write_bmp(filename.c_str(), width, height, 3, data);

	delete[] data;
}

unsigned char* Framebuffer::emptyImage(size_t width, size_t height)
{
	unsigned char* data = new unsigned char[3 * width*height];

	for (size_t i = 0; i < 3 * width*height; i += 3)
	{
		data[i + 0] = 255;
		data[i + 1] = 255;
		data[i + 2] = 255;
	}

	return data;
}