package com.github.chetangandhi.sphereWithLight;

import android.app.Activity;
import android.os.Bundle;
import android.view.Window;
import android.view.WindowManager;
import android.content.pm.ActivityInfo;
import android.graphics.Color;

public class MainActivity extends Activity {

    private SphereWithLight sphereWithLight = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Remove the ActionBar
        this.requestWindowFeature(Window.FEATURE_NO_TITLE);

        // Make Fullscreen
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);

        // Make window background red.
        this.getWindow().getDecorView().setBackgroundColor(Color.rgb(0, 0, 0));

        // Force activity window orientation to be Landscape
        this.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_SENSOR_LANDSCAPE);

        // Create the text view.
        this.sphereWithLight = new SphereWithLight(this);

        // Set textView as content view of the activity.
        setContentView(this.sphereWithLight);
    }
}
