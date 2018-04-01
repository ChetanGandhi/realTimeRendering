package com.github.chetangandhi.triangleAndSquareRotation;

import android.content.Context;

import android.opengl.GLSurfaceView;
import android.opengl.GLES32;
import android.opengl.Matrix;

import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.GestureDetector.OnDoubleTapListener;

import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

public class TriangleAndSquareRotation extends GLSurfaceView implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener {

    private final Context context;
    private GestureDetector gestureDetector = null;

    private float angleTriangle = 0.0f;
    private float angleSquare = 0.0f;
    private float speed = 0.1f;

    private int singleTapCounter = 1;
    private int vertexShaderObject = 0;
    private int fragmentShaderObject = 0;
    private int shaderProgramObject = 0;
    private int mvpUniform = 0;

    private int[] vaoTriangle = new int[1];
    private int[] vboTrianglePosition = new int[1];
    private int[] vboTriangleColor = new int[1];
    private int[] vaoSquare = new int[1];
    private int[] vboSquarePosition = new int[1];

    private float[] perspectiveProjectionMatrix = new float[16];

    TriangleAndSquareRotation(Context context) {
        super(context);

        this.context = context;

        this.setEGLContextClientVersion(3);
        this.setRenderer(this);
        this.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

        this.gestureDetector = new GestureDetector(context, this, null, false);
        this.gestureDetector.setOnDoubleTapListener(this);
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        String version = gl.glGetString(GL10.GL_VERSION);
        System.out.println("CG | OpenGL version: " + version);

        String glslVersion = gl.glGetString(GLES32.GL_SHADING_LANGUAGE_VERSION);
        System.out.println("CG | GLSL version: " + glslVersion);

        this.initialize(gl);
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        this.resize(width, height);
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        this.display();
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
        System.out.println("CG | " + this.context.getString(R.string.text_double_tap));
        return true;
    }

    @Override
    public boolean onDoubleTapEvent(MotionEvent event) {
        // Do nothing here as we have implemented onDoubleTap methid, just return true.
        return true;
    }

    @Override
    public boolean onSingleTapConfirmed(MotionEvent event) {
        System.out.println("CG | " + this.context.getString(R.string.text_single_tap_confirmed));

        singleTapCounter++;

        if(singleTapCounter < 1 || singleTapCounter > 9) {
            singleTapCounter = 1;
        }

        speed = 0.1f * (float)singleTapCounter;

        return true;
    }

    @Override
    public boolean onDown(MotionEvent event) {
        // Do nothing here as we have implemented onSingleTapConfirmed method, just return true.
        return true;
    }

    @Override
    public boolean onFling(MotionEvent eventStart, MotionEvent eventEnd, float velocityX, float velocityY) {
        System.out.println("CG | " + this.context.getString(R.string.text_fling));
        return true;
    }

    @Override
    public void onLongPress(MotionEvent event) {
        System.out.println("CG | " + this.context.getString(R.string.text_long_press));
    }

    @Override
    public void onShowPress(MotionEvent event) {
    }

    @Override
    public boolean onScroll(MotionEvent eventStart, MotionEvent eventEnd, float distanceX, float distanceY) {
        System.out.println("CG | " + this.context.getString(R.string.text_scroll));
        this.cleanUp();
        System.exit(0);
        return true;
    }

    @Override
    public boolean onSingleTapUp(MotionEvent event) {
        return true;
    }

    private void initialize(GL10 gl) {
        this.initializeVertexShaderObject();
        this.initializeFragmentShaderObject();
        this.initializeShaderProgramObject();
        this.initializeTriangleBuffers();
        this.initializeSquareBuffers();

        GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        GLES32.glEnable(GLES32.GL_DEPTH_TEST);

        // Disable face culling to see back side of object when rotated.
        // GLES32.glEnable(GL_CULL_FACE);

        GLES32.glDepthFunc(GLES32.GL_LEQUAL);

        Matrix.setIdentityM(this.perspectiveProjectionMatrix, 0);
    }

    private void initializeVertexShaderObject() {
        this.vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
        final String vertexShaderCode = String.format(
            "#version 320 es"
            + "\n"
            + "in vec4 vertexPosition;"
            + "in vec4 vertexColor;"
            + "\n"
            + "out vec4 outVertexColor;"
            + "\n"
            + "uniform mat4 mvpMatrix;"
            + "\n"
            + "void main(void)"
            + "{"
            + "   gl_Position = mvpMatrix * vertexPosition;"
            + "   outVertexColor = vertexColor;"
            + "}"
        );

        GLES32.glShaderSource(this.vertexShaderObject, vertexShaderCode);
        GLES32.glCompileShader(this.vertexShaderObject);

        int[] infoLogLength = new int[1];
        int[] shaderCompileStatus = new int[1];
        String infoLog = null;

        GLES32.glGetShaderiv(this.vertexShaderObject, GLES32.GL_COMPILE_STATUS, shaderCompileStatus, 0);

        if(shaderCompileStatus[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetShaderiv(this.vertexShaderObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);

            if(infoLogLength[0] > 0)
            {
                infoLog = GLES32.glGetShaderInfoLog(this.vertexShaderObject);
                System.out.println("CG | Vertex shader compilation log: " + infoLog);
                cleanUp();
                System.exit(1);
            }
        }
    }

    private void initializeFragmentShaderObject() {
        this.fragmentShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);

        final String fragmentShaderCode = String.format(
            "#version 320 es"
            + "\n"
            + "precision highp float;"
            + "\n"
            + "in vec4 outVertexColor;"
            + "\n"
            + "out vec4 fragmentColor;"
            + "\n"
            + "void main(void)"
            + "{"
            + "   fragmentColor = outVertexColor;"
            + "}"
        );

        GLES32.glShaderSource(this.fragmentShaderObject, fragmentShaderCode);
        GLES32.glCompileShader(this.fragmentShaderObject);

        int[] infoLogLength = new int[1];
        int[] shaderCompileStatus = new int[1];
        String infoLog = null;

        GLES32.glGetShaderiv(this.fragmentShaderObject, GLES32.GL_COMPILE_STATUS, shaderCompileStatus, 0);

        if(shaderCompileStatus[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetShaderiv(this.fragmentShaderObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);

            if(infoLogLength[0] > 0)
            {
                infoLog = GLES32.glGetShaderInfoLog(this.fragmentShaderObject);
                System.out.println("CG | Fragment shader compilation log: " + infoLog);
                cleanUp();
                System.exit(1);
            }
        }
    }

    private void initializeShaderProgramObject() {
        this.shaderProgramObject = GLES32.glCreateProgram();
        GLES32.glAttachShader(this.shaderProgramObject, this.vertexShaderObject);
        GLES32.glAttachShader(this.shaderProgramObject, this.fragmentShaderObject);

        // Bind the position attribute location before linking.
        GLES32.glBindAttribLocation(this.shaderProgramObject, GLESMacros.CG_ATTRIBUTE_VERTEX_POSITION, "vertexPosition");

        // Bind the color attribute location before linking.
        GLES32.glBindAttribLocation(this.shaderProgramObject, GLESMacros.CG_ATTRIBUTE_COLOR, "vertexColor");

        // Now link and check for error.
        GLES32.glLinkProgram(this.shaderProgramObject);

        int[] infoLogLength = new int[1];
        int[] shaderProgramLinkStatus = new int[1];
        String infoLog = null;

        GLES32.glGetProgramiv(this.shaderProgramObject, GLES32.GL_LINK_STATUS, shaderProgramLinkStatus, 0);

        if(shaderProgramLinkStatus[0] == GLES32.GL_FALSE)
        {
            GLES32.glGetProgramiv(this.shaderProgramObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLength, 0);

            if(infoLogLength[0] > 0)
            {
                infoLog = GLES32.glGetProgramInfoLog(this.shaderProgramObject);
                System.out.println("CG | Shader program link log: " + infoLog);
                cleanUp();
                System.exit(1);
            }
        }

        // After linking get the value of MVP uniform location from the shader program.
        mvpUniform = GLES32.glGetUniformLocation(this.shaderProgramObject, "mvpMatrix");
    }

    private void initializeTriangleBuffers() {
        final float[] triangleVertices = new float[]{
            0.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f
        };

        final float[] triangleColors = {
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f
        };

        GLES32.glGenVertexArrays(1, this.vaoTriangle, 0);
        GLES32.glBindVertexArray(this.vaoTriangle[0]);

        GLES32.glGenBuffers(1, this.vboTrianglePosition, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, this.vboTrianglePosition[0]);

        ByteBuffer verticesByteBuffer = ByteBuffer.allocateDirect(triangleVertices.length * 4);
        verticesByteBuffer.order(ByteOrder.nativeOrder());

        FloatBuffer verticesBuffer = verticesByteBuffer.asFloatBuffer();
        verticesBuffer.put(triangleVertices);
        verticesBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, triangleVertices.length * 4, verticesBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(GLESMacros.CG_ATTRIBUTE_VERTEX_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(GLESMacros.CG_ATTRIBUTE_VERTEX_POSITION);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        GLES32.glGenBuffers(1, this.vboTriangleColor, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, this.vboTriangleColor[0]);

        ByteBuffer colorByteBuffer = ByteBuffer.allocateDirect(triangleColors.length * 4);
        colorByteBuffer.order(ByteOrder.nativeOrder());

        FloatBuffer colorBuffer = colorByteBuffer.asFloatBuffer();
        colorBuffer.put(triangleColors);
        colorBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, triangleColors.length * 4, colorBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(GLESMacros.CG_ATTRIBUTE_COLOR, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(GLESMacros.CG_ATTRIBUTE_COLOR);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        GLES32.glBindVertexArray(0);
    }

    private void initializeSquareBuffers() {
        final float[] squareVertices = new float[]{
            -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f
        };

        GLES32.glGenVertexArrays(1, this.vaoSquare, 0);
        GLES32.glBindVertexArray(this.vaoSquare[0]);

        GLES32.glGenBuffers(1, this.vboSquarePosition, 0);
        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, this.vboSquarePosition[0]);

        ByteBuffer verticesByteBuffer = ByteBuffer.allocateDirect(squareVertices.length * 4);
        verticesByteBuffer.order(ByteOrder.nativeOrder());

        FloatBuffer verticesBuffer = verticesByteBuffer.asFloatBuffer();
        verticesBuffer.put(squareVertices);
        verticesBuffer.position(0);

        GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, squareVertices.length * 4, verticesBuffer, GLES32.GL_STATIC_DRAW);
        GLES32.glVertexAttribPointer(GLESMacros.CG_ATTRIBUTE_VERTEX_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
        GLES32.glEnableVertexAttribArray(GLESMacros.CG_ATTRIBUTE_VERTEX_POSITION);

        GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

        GLES32.glVertexAttrib3f(GLESMacros.CG_ATTRIBUTE_COLOR, 0.392156f, 0.584313f, 0.929411f);

        GLES32.glBindVertexArray(0);
    }

    private void update() {
        angleSquare -= speed;
        angleTriangle += speed;

        if(angleSquare <= -360.0f) {
            angleSquare = 0.0f;
        }

        if(angleTriangle >= 360.0f) {
            angleTriangle = 0.0f;
        }
    }

    private void display() {
        GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT | GLES32.GL_STENCIL_BUFFER_BIT);
        GLES32.glUseProgram(shaderProgramObject);

        this.drawTriangle();
        this.drawSquare();

        GLES32.glUseProgram(0);

        this.update();
        this.requestRender();
    }

    private void drawTriangle() {
        float[] modelViewMatrix = new float[16];
        float[] rotationMatrix = new float[16];
        float[] modelViewProjectionMatrix = new float[16];

        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(rotationMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);

        // Translate the modal view matrix.
        Matrix.translateM(modelViewMatrix, 0, -1.5f, 0.0f, -6.0f);
        Matrix.rotateM(rotationMatrix, 0, angleTriangle, 0.0f, 1.0f, 0.0f);

        // Rotate after transformation of modelViewMatrix.
        Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0, rotationMatrix, 0);

        // Multiply modelViewMatrix and perspectiveProjectionMatrix to get modelViewProjectionMatrix
        // Oder of multiplication is very important projectionMatrix * modelMatrix * viewMatrix
        // As we have model and view matrix combined, we just have to multiply projectionMatrix and modelViewMatrix
        Matrix.multiplyMM(modelViewProjectionMatrix, 0, this.perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

        // Pass modelViewProjectionMatrix to vertex shader in 'mvpMatrix' variable defined in shader.
        GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

        // Now bind the voaTriangle to which we want to use
        GLES32.glBindVertexArray(vaoTriangle[0]);

        // Draw the triangle
        // 3 is number of vertices in the array i.e. element count in triangleVertices divide by 3 (x, y, z) component
        GLES32.glDrawArrays(GLES32.GL_TRIANGLES, 0, 3);

        // unbind the vaoTriangle
        GLES32.glBindVertexArray(0);
    }

    private void drawSquare() {
        float[] modelViewMatrix = new float[16];
        float[] rotationMatrix = new float[16];
        float[] modelViewProjectionMatrix = new float[16];

        Matrix.setIdentityM(modelViewMatrix, 0);
        Matrix.setIdentityM(rotationMatrix, 0);
        Matrix.setIdentityM(modelViewProjectionMatrix, 0);

        // Translate the modal view matrix.
        Matrix.translateM(modelViewMatrix, 0, 1.5f, 0.0f, -6.0f);
        Matrix.rotateM(rotationMatrix, 0, angleSquare, 1.0f, 0.0f, 0.0f);

        // Rotate after transformation of modelViewMatrix.
        Matrix.multiplyMM(modelViewMatrix, 0, modelViewMatrix, 0, rotationMatrix, 0);

        // Multiply modelViewMatrix and perspectiveProjectionMatrix to get modelViewProjectionMatrix
        // Oder of multiplication is very important projectionMatrix * modelMatrix * viewMatrix
        // As we have model and view matrix combined, we just have to multiply projectionMatrix and modelViewMatrix
        Matrix.multiplyMM(modelViewProjectionMatrix, 0, this.perspectiveProjectionMatrix, 0, modelViewMatrix, 0);

        // Pass modelViewProjectionMatrix to vertex shader in 'mvpMatrix' variable defined in shader.
        GLES32.glUniformMatrix4fv(mvpUniform, 1, false, modelViewProjectionMatrix, 0);

        // Now bind the vaoSquare to which we want to use
        GLES32.glBindVertexArray(vaoSquare[0]);

        // Draw the square
        // 4 is number of vertices in the array i.e. element count in squareVertices divide by 3 (x, y, z) component
        GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);

        // unbind the vaoSquare
        GLES32.glBindVertexArray(0);
    }

    private void resize(int width, int height) {
        if(height == 0) {
            height = 1;
        }

        GLES32.glViewport(0, 0, width, height);
        Matrix.perspectiveM(this.perspectiveProjectionMatrix, 0, 45.0f, (float)width / (float)height, 1.0f, 100.0f);
    }

    private void cleanUp() {
        if(this.vaoTriangle[0] != 0) {
            GLES32.glDeleteVertexArrays(1, this.vaoTriangle, 0);
            this.vaoTriangle[0] = 0;
        }

        if(this.vboTrianglePosition[0] != 0)
        {
            GLES32.glDeleteBuffers(1, this.vboTrianglePosition, 0);
            this.vboTrianglePosition[0] = 0;
        }

        if(this.vboTriangleColor[0] != 0)
        {
            GLES32.glDeleteBuffers(1, this.vboTriangleColor, 0);
            this.vboTriangleColor[0] = 0;
        }

        if(this.vaoSquare[0] != 0) {
            GLES32.glDeleteVertexArrays(1, this.vaoSquare, 0);
            this.vaoSquare[0] = 0;
        }

        if(this.vboSquarePosition[0] != 0)
        {
            GLES32.glDeleteBuffers(1, this.vboSquarePosition, 0);
            this.vboSquarePosition[0] = 0;
        }

        if(this.shaderProgramObject != 0)
        {
            if(this.vertexShaderObject != 0)
            {
                GLES32.glDetachShader(this.shaderProgramObject, this.vertexShaderObject);
            }

            if(this.fragmentShaderObject != 0)
            {
                GLES32.glDetachShader(this.shaderProgramObject, this.fragmentShaderObject);
            }
        }

        if(this.vertexShaderObject != 0)
        {
            GLES32.glDeleteShader(this.vertexShaderObject);
            this.vertexShaderObject = 0;
        }

        if(this.fragmentShaderObject != 0)
        {
            GLES32.glDeleteShader(this.fragmentShaderObject);
            this.fragmentShaderObject = 0;
        }

        if(this.shaderProgramObject != 0)
        {
            GLES32.glDeleteProgram(this.shaderProgramObject);
            this.shaderProgramObject = 0;
        }

        GLES32.glUseProgram(0);
    }
}
