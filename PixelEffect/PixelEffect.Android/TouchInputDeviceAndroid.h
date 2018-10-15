#pragma once

#include <InputDevice.h>
#include <TouchInputDevice.h>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

class TouchInputDeviceAndroid : public TouchInputDevice
{
private:
	vector<TouchInputDeviceHandler*> handlers;
	map<int, TouchItem*> touchItems;

public:
	void update(long long elapsedTime);

	map<int, TouchItem*> getTouchItems();

	void touchPointerIn(TouchEvent e);
	void touchPointerOut(TouchEvent e);

	void touchDown(TouchEvent e);
	void touchUp(TouchEvent e);
	void touchMove(TouchEvent e);	

	void addHandler(TouchInputDeviceHandler* handler);
	void removeHandler(TouchInputDeviceHandler* handler);
};

