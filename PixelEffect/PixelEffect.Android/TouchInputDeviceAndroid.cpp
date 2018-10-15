#include "TouchInputDeviceAndroid.h"

map<int, TouchItem*> TouchInputDeviceAndroid::getTouchItems() 
{
	return touchItems;
}

bool hasChanged(TouchItem* touchItem, TouchPosition position)
{
	if (touchItem->currentPosition.x == position.x && touchItem->currentPosition.y == position.y)
		return false;

	return true;
}

void TouchInputDeviceAndroid::addHandler(TouchInputDeviceHandler* handler)
{
	handlers.push_back(handler);
}

void TouchInputDeviceAndroid::removeHandler(TouchInputDeviceHandler* handler) 
{
	vector<TouchInputDeviceHandler*>::iterator item = std::find(handlers.begin(), handlers.end(), handler);

	if (item != handlers.end())
		handlers.erase(item);
}

void TouchInputDeviceAndroid::update(long long elapsedTime)
{	
	for (auto handler : handlers)
		handler->updateTouchItems(&touchItems);
}

void TouchInputDeviceAndroid::touchPointerIn(TouchEvent e) 
{
	for (size_t i = 0; i < handlers.size(); i++)
		handlers[i]->onTouchPointerIn(e);
}
void TouchInputDeviceAndroid::touchPointerOut(TouchEvent e) 
{
	for (size_t i = 0; i < handlers.size(); i++)
		handlers[i]->onTouchPointerOut(e);
}

void TouchInputDeviceAndroid::touchDown(TouchEvent e) 
{	
	TouchItem* item = new TouchItem { e.pointerId, e.pointerSize, e.position, e.position , e.position };
	pair<int, TouchItem*> touchItemPair = pair<int, TouchItem*>{ e.pointerId, item };
	touchItems.insert(touchItemPair);

	for (size_t i = 0; i < handlers.size(); i++)
		handlers[i]->onTouchDown(item);
}

void TouchInputDeviceAndroid::touchUp(TouchEvent e) 
{
	TouchItem* item = touchItems.at(e.pointerId);

	for (size_t i = 0; i < handlers.size(); i++)
		handlers[i]->onTouchUp(item);

	touchItems.erase(e.pointerId);
	delete item;
}

void TouchInputDeviceAndroid::touchMove(TouchEvent e) 
{
	TouchItem* item = touchItems.at(e.pointerId);
	
	if ( ! hasChanged(item, e.position) )
		return;

	item->previousPosition = item->currentPosition;
	item->currentPosition = e.position;

	item->pointerSize = e.pointerSize;

	for (size_t i = 0; i < handlers.size(); i++)
		handlers[i]->onTouchMove(item);
}