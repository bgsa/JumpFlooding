#pragma once

#include <DisplayDevice.h>

class DisplayDeviceAndroid : public DisplayDevice
{
public:
	void init(int width, int height);
	void swapBuffer();
};

