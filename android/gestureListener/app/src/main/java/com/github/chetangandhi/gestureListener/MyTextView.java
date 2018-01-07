package com.github.chetangandhi.gestureListener;

import android.widget.TextView;
import android.content.Context;
import android.graphics.Color;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.GestureDetector.OnDoubleTapListener;

public class MyTextView extends TextView implements OnGestureListener, OnDoubleTapListener {

    private GestureDetector gestureDetector = null;

    MyTextView(Context context) {
        super(context);

        this.setText(R.string.text_message);
        this.setTextSize(60);
        this.setTextColor(Color.GREEN);
        this.setGravity(Gravity.CENTER);

        this.gestureDetector = new GestureDetector(context, this, null, false);
        this.gestureDetector.setOnDoubleTapListener(this);
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        int eventAction = event.getAction();

        if (!this.gestureDetector.onTouchEvent(event)) {
            super.onTouchEvent(event);
        }

        return true;
    }

    @Override
    public boolean onDoubleTap(MotionEvent event) {
        this.setText(R.string.text_double_tap);
        return true;
    }

    @Override
    public boolean onDoubleTapEvent(MotionEvent event) {
        // Do nothing here as we have implemented onDoubleTap methid, just return true.
        return true;
    }

    @Override
    public boolean onSingleTapConfirmed(MotionEvent event) {
        this.setText(R.string.text_single_tap_confirmed);
        return true;
    }

    @Override
    public boolean onDown(MotionEvent event) {
        // Do nothing here as we have implemented onSingleTapConfirmed method, just return true.
        return true;
    }

    @Override
    public boolean onFling(MotionEvent eventStart, MotionEvent eventEnd, float velocityX, float velocityY) {
        this.setText(R.string.text_fling);
        return true;
    }

    @Override
    public void onLongPress(MotionEvent event) {
        this.setText(R.string.text_long_press);
    }

    @Override
    public void onShowPress(MotionEvent event) {
    }

    @Override
    public boolean onScroll(MotionEvent eventStart, MotionEvent eventEnd, float distanceX, float distanceY) {
        this.setText(R.string.text_scroll);
        return true;
    }

    @Override
    public boolean onSingleTapUp(MotionEvent event) {
        return true;
    }

}
