package com.pixeleffect;

import android.app.Activity;
import android.content.res.AssetManager;
import android.os.Bundle;

public class MainActivity extends Activity
{
    OpenGLView mView;

    @Override
    protected void onCreate(Bundle icicle)
    {
        super.onCreate(icicle);

        AssetManager assetManager = getResources().getAssets();
        NativeInterface.initAssetManager(assetManager);

        mView = new OpenGLView(getApplication());
        setContentView(mView);
    }

    @Override
    protected void onPause()
    {
        super.onPause();
        mView.onPause();
    }

    @Override
    protected void onResume()
    {
        super.onResume();
        mView.onResume();
    }

}
