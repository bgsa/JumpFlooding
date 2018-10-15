package com.pixeleffect;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.DisplayMetrics;
import android.view.MotionEvent;
import android.view.WindowManager;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import static android.content.Context.WINDOW_SERVICE;

public class OpenGLView extends GLSurfaceView
{
    private static Renderer renderer;

    public OpenGLView(Context context)
    {
        super(context);

        // Pick an EGLConfig with RGB8 color, 16-bit depth, no stencil, supporting OpenGL ES 2.0 or later backwards-compatible versions.
        setEGLConfigChooser(8, 8, 8, 0, 16, 0);
        setEGLContextClientVersion(3);
        setPreserveEGLContextOnPause(true);

        renderer = new Renderer(context);
        setRenderer(renderer);
    }

    @Override
    public boolean onTouchEvent(MotionEvent e)
    {
        int action = e.getActionMasked();
        int pointerIndex = e.getActionIndex();

        final int pointerId = e.getPointerId(pointerIndex);
        final float pointerSize = e.getSize(pointerIndex);
        final float x = e.getX(pointerIndex);
        final float y = renderer.getHeight() - e.getY(pointerIndex);

        switch (action)
        {
            case MotionEvent.ACTION_MOVE:
                for (int i = 0; i < e.getPointerCount(); i ++) {
                    final int pointerIdi = e.getPointerId(i);
                    final float pointerSizei = e.getSize(i);
                    final float xi = e.getX(i);
                    final float yi = renderer.getHeight() - e.getY(i);

                    queueEvent(new Runnable(){
                        public void run() {
                            NativeInterface.touchMove(pointerIdi, pointerSizei, xi ,yi);
                        }});
                }
                break;

            case MotionEvent.ACTION_POINTER_DOWN:
                queueEvent(new Runnable(){
                    public void run() {
                        NativeInterface.touchPointerIn(pointerId, pointerSize, x ,y);
                    }});

            case MotionEvent.ACTION_DOWN:
                queueEvent(new Runnable(){
                    public void run() {
                        NativeInterface.touchDown(pointerId, pointerSize, x ,y);
                    }});
                break;

            case MotionEvent.ACTION_POINTER_UP:
                queueEvent(new Runnable(){
                    public void run() {
                        NativeInterface.touchPointerOut(pointerId, pointerSize, x ,y);
                    }});

            case MotionEvent.ACTION_UP:
                queueEvent(new Runnable(){
                    public void run() {
                        NativeInterface.touchUp(pointerId, pointerSize, x ,y);
                    }});

                break;
        }

        return true;
    }

    private static class Renderer implements GLSurfaceView.Renderer
    {
        private Context context;
        private float width = 0;
        private float height = 0;

        public float getWidth() {
            return width;
        }

        public float getHeight() {
            return height;
        }

        public Renderer(Context context)
        {
            this.context = context;
        }

        public void initDisplayDevice()
        {
            DisplayMetrics dm = new DisplayMetrics();
            WindowManager windowManager = (WindowManager) context.getSystemService(WINDOW_SERVICE);
            windowManager.getDefaultDisplay().getMetrics(dm);

            width = dm.widthPixels;
            height = dm.heightPixels;

            NativeInterface.initDisplayDevice((int)width, (int)height);
        }

        public void onSurfaceCreated(GL10 gl, EGLConfig config)
        {
            initDisplayDevice();
            NativeInterface.init();
        }

        public void onDrawFrame(GL10 gl)
        {
            NativeInterface.step();
        }

        public void onSurfaceChanged(GL10 gl, int width, int height)
        {
            NativeInterface.resize(width, height);
        }

    }
}
