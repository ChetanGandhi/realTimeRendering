package com.github.chetangandhi.fullscreenMyTextView;

import android.widget.TextView;
import android.content.Context;
import android.graphics.Color;
import android.view.Gravity;

public class MyView extends TextView {

    public MyView(Context context) {
        super(context);

        this.setText("Hello from Class!!");
        this.setTextSize(60);
        this.setTextColor(Color.GREEN);
        this.setGravity(Gravity.CENTER);
    }
}
