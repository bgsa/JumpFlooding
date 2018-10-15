package com.pixeleffect;

import android.content.res.AssetManager;

public class NativeInterface
{
     static
     {
          System.loadLibrary("pugixml");
          System.loadLibrary("z");
          System.loadLibrary("png");
          System.loadLibrary("freetype");
          System.loadLibrary("PixelEffect");
     }

     public static native void initDisplayDevice(int width, int height);
     public static native void init();
     public static native void initAssetManager(AssetManager mgr);
     public static native void resize(int width, int height);
     public static native void step();

     public static native void touchPointerIn(int pointerId, float pointerSize, float x ,float y);
     public static native void touchPointerOut(int pointerId, float pointerSize, float x ,float y);
     public static native void touchDown(int pointerId, float pointerSize, float x ,float y);
     public static native void touchUp(int pointerId, float pointerSize, float x ,float y);
     public static native void touchMove(int pointerId, float pointerSize, float x ,float y);
}
