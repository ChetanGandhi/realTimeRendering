package com.github.chetangandhi.fullscreenMyTextView;

import android.widget.TextView;
import android.content.Context;
import android.graphics.Color;
import android.view.Gravity;

public class MyTextView extends TextView {

    MyTextView(Context context) {
        super(context);

        this.setText(R.string.text_message);
        this.setTextSize(60);
        this.setTextColor(Color.GREEN);
        this.setGravity(Gravity.CENTER);
    }
}
