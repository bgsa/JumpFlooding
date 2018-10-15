#include "GLConfig.h"
#include "Log.h"
#include "DisplayDeviceAndroid.h"
#include "TouchInputDeviceAndroid.h"
#include "Renderer.h"
#include "RendererSettings.h"
#include "FileManagerAndroid.h"

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

Renderer renderer;
DisplayDeviceAndroid* display = new DisplayDeviceAndroid;
TouchInputDeviceAndroid* touchInputDevice = new TouchInputDeviceAndroid;

extern "C" {
	JNIEXPORT void JNICALL Java_com_pixeleffect_NativeInterface_initDisplayDevice(JNIEnv* env, jobject obj, jint width, jint height);
	JNIEXPORT void JNICALL Java_com_pixeleffect_NativeInterface_init(JNIEnv* env, jobject obj);
	JNIEXPORT void JNICALL Java_com_pixeleffect_NativeInterface_initAssetManager(JNIEnv* env, jobject obj, jobject assetManager);
	JNIEXPORT void JNICALL Java_com_pixeleffect_NativeInterface_resize(JNIEnv* env, jobject obj, jint width, jint height);
	JNIEXPORT void JNICALL Java_com_pixeleffect_NativeInterface_step(JNIEnv* env, jobject obj);

	// INPUT HANDLE
	JNIEXPORT void JNICALL Java_com_pixeleffect_NativeInterface_touchPointerIn(JNIEnv* env, jobject obj, jint pointerId, jfloat pointerSize, jfloat x, jfloat y);
	JNIEXPORT void JNICALL Java_com_pixeleffect_NativeInterface_touchPointerOut(JNIEnv* env, jobject obj, jint pointerId, jfloat pointerSize, jfloat x, jfloat y);
	JNIEXPORT void JNICALL Java_com_pixeleffect_NativeInterface_touchDown(JNIEnv* env, jobject obj, jint pointerId, jfloat pointerSize, jfloat x, jfloat y);
	JNIEXPORT void JNICALL Java_com_pixeleffect_NativeInterface_touchUp(JNIEnv* env, jobject obj, jint pointerId, jfloat pointerSize, jfloat x, jfloat y);
	JNIEXPORT void JNICALL Java_com_pixeleffect_NativeInterface_touchMove(JNIEnv* env, jobject obj, jint pointerId, jfloat pointerSize, jfloat x, jfloat y);
};

JNIEXPORT void JNICALL
Java_com_pixeleffect_NativeInterface_initDisplayDevice(JNIEnv* env, jobject obj, jint width, jint height)
{
	RendererSettings::getInstance()->setWidth(width);
	RendererSettings::getInstance()->setHeight(height);

	display->init(width, height);
}

JNIEXPORT void JNICALL
Java_com_pixeleffect_NativeInterface_init(JNIEnv* env, jobject obj)
{
	renderer.addInputDevice(touchInputDevice);
	renderer.init(display);
	touchInputDevice->addHandler(&renderer);
	renderer.start();
}

JNIEXPORT void JNICALL
Java_com_pixeleffect_NativeInterface_initAssetManager(JNIEnv* env, jobject obj, jobject assetManagerObj)
{
	AAssetManager *assetManager = AAssetManager_fromJava(env, assetManagerObj);

	FileManagerAndroid fileManager; 
	fileManager.init(assetManager);
}

JNIEXPORT void JNICALL
Java_com_pixeleffect_NativeInterface_resize(JNIEnv* env, jobject obj, jint width, jint height)
{
	renderer.resize(width, height);
}

JNIEXPORT void JNICALL
Java_com_pixeleffect_NativeInterface_step(JNIEnv* env, jobject obj)
{
	renderer.update();
	renderer.render();
}


// INPUT HANDLE
JNIEXPORT void JNICALL
Java_com_pixeleffect_NativeInterface_touchPointerIn(JNIEnv* env, jobject obj, jint pointerId, jfloat pointerSize, jfloat x, jfloat y)
{
	TouchEvent touchEvent = TouchEvent{ pointerId, pointerSize,{ x, y } };
	touchInputDevice->touchPointerIn(touchEvent);
}

JNIEXPORT void JNICALL
Java_com_pixeleffect_NativeInterface_touchPointerOut(JNIEnv* env, jobject obj, jint pointerId, jfloat pointerSize, jfloat x, jfloat y)
{
	TouchEvent touchEvent = TouchEvent{ pointerId, pointerSize,{ x, y } };
	touchInputDevice->touchPointerOut(touchEvent);
}

JNIEXPORT void JNICALL 
Java_com_pixeleffect_NativeInterface_touchDown(JNIEnv* env, jobject obj, jint pointerId, jfloat pointerSize, jfloat x, jfloat y)
{
	TouchEvent touchEvent = TouchEvent{ pointerId, pointerSize, {x, y} };
	touchInputDevice->touchDown(touchEvent);
}

JNIEXPORT void JNICALL
Java_com_pixeleffect_NativeInterface_touchUp(JNIEnv* env, jobject obj, jint pointerId, jfloat pointerSize, jfloat x, jfloat y)
{
	TouchEvent touchEvent = TouchEvent{ pointerId, pointerSize,{ x, y } };
	touchInputDevice->touchUp(touchEvent);
}

JNIEXPORT void JNICALL
Java_com_pixeleffect_NativeInterface_touchMove(JNIEnv* env, jobject obj, jint pointerId, jfloat pointerSize, jfloat x, jfloat y)
{
	TouchEvent touchEvent = TouchEvent{ pointerId, pointerSize,{ x, y } };
	touchInputDevice->touchMove(touchEvent);
}
