#pragma once

#include "GlobalHeader.h"
#include "RendererSettings.h"

class Framebuffer
{
private:


public:

	static unsigned char* getFramebuffer(GLenum framebuffer);

	static void saveFrambebuffer(std::string filename, GLenum framebuffer);

	static unsigned char* emptyImage(size_t width, size_t height);

};
